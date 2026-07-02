#include "voiceclient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkDatagram>
#include <QUrl>
#include <QWebSocket>

VoiceClient::VoiceClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QWebSocket())
    , m_udpSocket(new QUdpSocket(this))
    , m_heartbeatTimer(new QTimer(this))
{
    connect(m_socket, &QWebSocket::connected, this, &VoiceClient::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &VoiceClient::onSocketDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &VoiceClient::onTextMessageReceived);
    connect(m_socket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        log("Voice socket error: " + m_socket->errorString());
        emit voiceError(m_socket->errorString());
    });

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &VoiceClient::onUdpReadyRead);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &VoiceClient::sendHeartbeat);
}

VoiceClient::~VoiceClient()
{
    disconnectVoice();
    m_socket->deleteLater();
}

void VoiceClient::log(const QString &line)
{
    emit voiceLog(line);
}

void VoiceClient::connectVoice(const QString &endpoint, const QString &token, const QString &guildId,
                                const QString &userId, const QString &sessionId)
{
    m_token = token;
    m_guildId = guildId;
    m_userId = userId;
    m_sessionId = sessionId;

    // IMPORTANT: use the endpoint exactly as given, port included. Discord
    // assigns a specific port (2053, 2083, etc, not always 443) that routes
    // to the correct backend for this session - stripping it and forcing
    // 443 was landing us on the wrong server entirely.
    QString url = QString("wss://%1/?v=9").arg(endpoint);
    log("Connecting to voice gateway: " + url);
    m_socket->open(QUrl(url));
}

void VoiceClient::disconnectVoice()
{
    m_heartbeatTimer->stop();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->close();
    }
    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
        m_udpSocket->close();
    }
}

void VoiceClient::onSocketConnected()
{
    log("Voice WebSocket connected");
}

void VoiceClient::onSocketDisconnected()
{
    log(QString("Voice WebSocket disconnected - close code: %1, reason: \"%2\"")
            .arg(static_cast<int>(m_socket->closeCode()))
            .arg(m_socket->closeReason()));
    m_heartbeatTimer->stop();
}

void VoiceClient::onTextMessageReceived(const QString &message)
{
    QJsonObject payload = QJsonDocument::fromJson(message.toUtf8()).object();
    handlePayload(payload);
}

void VoiceClient::handlePayload(const QJsonObject &payload)
{
    int op = payload.value("op").toInt(-1);
    QJsonObject data = payload.value("d").toObject();

    if (payload.contains("seq") && !payload.value("seq").isNull()) {
        m_lastVoiceSequence = payload.value("seq").toVariant().toLongLong();
    }

    switch (op) {
    case Hello: {
        int intervalMs = data.value("heartbeat_interval").toInt();
        log(QString("Voice Hello - heartbeat_interval=%1ms, sending Identify").arg(intervalMs));
        m_heartbeatTimer->start(intervalMs);
        sendIdentify();
        break;
    }
    case VReady: {
        m_ssrc = static_cast<quint32>(data.value("ssrc").toVariant().toUInt());
        m_voiceServerIp = QHostAddress(data.value("ip").toString());
        m_voiceServerPort = static_cast<quint16>(data.value("port").toInt());

        m_availableModes.clear();
        for (const QJsonValue &v : data.value("modes").toArray()) {
            m_availableModes.append(v.toString());
        }

        log(QString("Voice Ready - ssrc=%1 ip=%2 port=%3 modes=[%4]")
                .arg(m_ssrc)
                .arg(m_voiceServerIp.toString())
                .arg(m_voiceServerPort)
                .arg(m_availableModes.join(", ")));

        performIpDiscovery();
        break;
    }
    case SessionDescription: {
        m_selectedMode = data.value("mode").toString();
        m_secretKey.clear();
        for (const QJsonValue &v : data.value("secret_key").toArray()) {
            m_secretKey.append(static_cast<char>(v.toInt()));
        }
        log(QString("Session Description received - mode=%1, secret_key=%2 bytes")
                .arg(m_selectedMode).arg(m_secretKey.size()));
        log("Voice handshake complete - ready for audio (not yet implemented, Phase B)");
        emit voiceHandshakeComplete();
        break;
    }
    case VHeartbeatAck: {
        break;
    }
    default:
        log(QString("Unhandled voice opcode: %1").arg(op));
        break;
    }
}

void VoiceClient::sendIdentify()
{
    log(QString("Sending voice Identify - server_id=%1, user_id=%2, session_id=%3, token_length=%4")
            .arg(m_guildId, m_userId, m_sessionId).arg(m_token.length()));

    QJsonObject data{
        {"server_id", m_guildId},
        {"user_id", m_userId},
        {"session_id", m_sessionId},
        {"token", m_token},
        {"video", false},
        {"max_dave_protocol_version", 1}
    };

    send(QJsonObject{
        {"op", Identify},
        {"d", data}
    });
}

void VoiceClient::sendHeartbeat()
{
    QJsonObject data{
        {"t", QDateTime::currentMSecsSinceEpoch()},
        {"seq_ack", m_lastVoiceSequence}
    };

    send(QJsonObject{
        {"op", Heartbeat},
        {"d", data}
    });
}

void VoiceClient::send(const QJsonObject &payload)
{
    m_socket->sendTextMessage(QJsonDocument(payload).toJson(QJsonDocument::Compact));
}

void VoiceClient::performIpDiscovery()
{
    QByteArray packet(74, static_cast<char>(0));
    packet[0] = 0x00; packet[1] = 0x01;
    packet[2] = 0x00; packet[3] = 0x46;
    packet[4] = static_cast<char>((m_ssrc >> 24) & 0xFF);
    packet[5] = static_cast<char>((m_ssrc >> 16) & 0xFF);
    packet[6] = static_cast<char>((m_ssrc >> 8) & 0xFF);
    packet[7] = static_cast<char>(m_ssrc & 0xFF);

    if (m_udpSocket->state() == QAbstractSocket::UnconnectedState) {
        m_udpSocket->bind();
    }

    log(QString("Sending UDP IP discovery packet to %1:%2")
            .arg(m_voiceServerIp.toString()).arg(m_voiceServerPort));
    m_udpSocket->writeDatagram(packet, m_voiceServerIp, m_voiceServerPort);
}

void VoiceClient::onUdpReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QByteArray reply = datagram.data();

        if (reply.size() < 74) {
            log(QString("Unexpected UDP reply size: %1 bytes (expected 74)").arg(reply.size()));
            continue;
        }

        QByteArray addressBytes = reply.mid(8, 64);
        int nullIndex = addressBytes.indexOf('\0');
        QString externalIp = QString::fromLatin1(
            nullIndex >= 0 ? addressBytes.left(nullIndex) : addressBytes);

        quint16 externalPort = static_cast<quint16>(
            (static_cast<unsigned char>(reply[72]) << 8) | static_cast<unsigned char>(reply[73]));

        log(QString("IP discovery response - external address seen as %1:%2")
                .arg(externalIp).arg(externalPort));

        sendSelectProtocol(externalIp, externalPort);
        return;
    }
}

void VoiceClient::sendSelectProtocol(const QString &externalIp, quint16 externalPort)
{
    QString mode = m_availableModes.contains("aead_xchacha20_poly1305_rtpsize")
        ? "aead_xchacha20_poly1305_rtpsize"
        : (m_availableModes.isEmpty() ? QString("aead_xchacha20_poly1305_rtpsize") : m_availableModes.first());

    log("Selecting voice protocol/mode: " + mode);

    QJsonObject protocolData{
        {"address", externalIp},
        {"port", externalPort},
        {"mode", mode}
    };

    send(QJsonObject{
        {"op", SelectProtocol},
        {"d", QJsonObject{
            {"protocol", "udp"},
            {"data", protocolData}
        }}
    });
}
