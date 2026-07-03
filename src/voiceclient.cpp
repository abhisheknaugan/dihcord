#include "voiceclient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkDatagram>
#include <QUrl>
#include <QWebSocket>
#include <sodium.h>

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

    connect(&m_audio, &AudioEngine::encodedFrameReady, this, &VoiceClient::sendEncryptedAudioFrame);
    connect(&m_audio, &AudioEngine::audioError, this, [this](const QString &reason) {
        log("Audio error: " + reason);
        emit voiceError(reason);
    });

    if (sodium_init() < 0) {
        log("libsodium failed to initialize");
    }
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

    QString url = QString("wss://%1/?v=9").arg(endpoint);
    log("Connecting to voice gateway: " + url);
    m_socket->open(QUrl(url));
}

void VoiceClient::disconnectVoice()
{
    m_heartbeatTimer->stop();
    m_audio.stopCapture();
    m_audio.stopPlayback();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->close();
    }
    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
        m_udpSocket->close();
    }
}

void VoiceClient::setMuted(bool muted)
{
    m_audio.setMuted(muted);
}

void VoiceClient::setDeafened(bool deafened)
{
    m_audio.setDeafened(deafened);
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
    m_audio.stopCapture();
    m_audio.stopPlayback();
    emit voiceDisconnected();
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
        log("Voice handshake complete - starting real audio");
        emit voiceHandshakeComplete();

        m_audio.startCapture();
        m_audio.startPlayback();
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

        log(QString("UDP packet received: %1 bytes from %2:%3, first byte=0x%4")
                .arg(reply.size())
                .arg(datagram.senderAddress().toString())
                .arg(datagram.senderPort())
                .arg(static_cast<unsigned char>(reply.isEmpty() ? 0 : reply[0]), 2, 16, QChar('0')));

        if (reply.size() == 74 && static_cast<unsigned char>(reply[0]) == 0x00
            && static_cast<unsigned char>(reply[1]) == 0x02) {
            QByteArray addressBytes = reply.mid(8, 64);
            int nullIndex = addressBytes.indexOf('\0');
            QString externalIp = QString::fromLatin1(
                nullIndex >= 0 ? addressBytes.left(nullIndex) : addressBytes);
            quint16 externalPort = static_cast<quint16>(
                (static_cast<unsigned char>(reply[72]) << 8) | static_cast<unsigned char>(reply[73]));

            log(QString("IP discovery response - external address seen as %1:%2")
                    .arg(externalIp).arg(externalPort));
            sendSelectProtocol(externalIp, externalPort);
            continue;
        }

        handleIncomingAudioPacket(reply);
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

void VoiceClient::sendEncryptedAudioFrame(const QByteArray &opusData)
{
    if (m_secretKey.size() != crypto_aead_xchacha20poly1305_ietf_KEYBYTES) {
        return;
    }

    m_rtpSequence++;
    m_rtpTimestamp += 960;

    QByteArray rtpHeader(12, static_cast<char>(0));
    rtpHeader[0] = static_cast<char>(0x80);
    rtpHeader[1] = static_cast<char>(0x78);
    rtpHeader[2] = static_cast<char>((m_rtpSequence >> 8) & 0xFF);
    rtpHeader[3] = static_cast<char>(m_rtpSequence & 0xFF);
    rtpHeader[4] = static_cast<char>((m_rtpTimestamp >> 24) & 0xFF);
    rtpHeader[5] = static_cast<char>((m_rtpTimestamp >> 16) & 0xFF);
    rtpHeader[6] = static_cast<char>((m_rtpTimestamp >> 8) & 0xFF);
    rtpHeader[7] = static_cast<char>(m_rtpTimestamp & 0xFF);
    rtpHeader[8] = static_cast<char>((m_ssrc >> 24) & 0xFF);
    rtpHeader[9] = static_cast<char>((m_ssrc >> 16) & 0xFF);
    rtpHeader[10] = static_cast<char>((m_ssrc >> 8) & 0xFF);
    rtpHeader[11] = static_cast<char>(m_ssrc & 0xFF);

    m_nonceCounter++;
    unsigned char nonceBytes[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES] = {0};
    std::memcpy(nonceBytes, &m_nonceCounter, sizeof(uint32_t));

    QByteArray ciphertext(opusData.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES, static_cast<char>(0));
    unsigned long long ciphertextLen = 0;

    crypto_aead_xchacha20poly1305_ietf_encrypt(
        reinterpret_cast<unsigned char *>(ciphertext.data()), &ciphertextLen,
        reinterpret_cast<const unsigned char *>(opusData.constData()), opusData.size(),
        reinterpret_cast<const unsigned char *>(rtpHeader.constData()), rtpHeader.size(),
        nullptr,
        nonceBytes,
        reinterpret_cast<const unsigned char *>(m_secretKey.constData()));

    QByteArray packet = rtpHeader + ciphertext.left(static_cast<int>(ciphertextLen));
    QByteArray nonceTrailer(4, static_cast<char>(0));
    std::memcpy(nonceTrailer.data(), &m_nonceCounter, sizeof(uint32_t));
    packet += nonceTrailer;

    m_udpSocket->writeDatagram(packet, m_voiceServerIp, m_voiceServerPort);

    m_sentFrameCount++;
    if (m_sentFrameCount % 50 == 0) {
        log(QString("Sent %1 audio frames so far").arg(m_sentFrameCount));
    }
}

void VoiceClient::handleIncomingAudioPacket(const QByteArray &packet)
{
    if (packet.size() < 44) {
        log(QString("Audio packet dropped: too small (%1 bytes)").arg(packet.size()));
        return;
    }
    if (m_secretKey.size() != crypto_aead_xchacha20poly1305_ietf_KEYBYTES) {
        log("Audio packet dropped: no secret key yet");
        return;
    }

    const auto *data = reinterpret_cast<const unsigned char *>(packet.constData());

    if (((data[0] >> 6) & 0x03) != 2) {
        log(QString("Audio packet dropped: RTP version mismatch (byte0=0x%1)")
                .arg(data[0], 2, 16, QChar('0')));
        return;
    }
    if ((data[1] & 0x7F) != 120) {
        log(QString("Audio packet dropped: payload type %1 (expected 120/Opus)").arg(data[1] & 0x7F));
        return;
    }

    uint32_t senderSsrc = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
    if (senderSsrc == m_ssrc) {
        log("Audio packet dropped: matches our own ssrc (echo)");
        return;
    }

    int csrcCount = data[0] & 0x0F;              // low 4 bits of first byte
    bool hasExtension = (data[0] & 0x10) != 0;
    int headerSize = 12 + (csrcCount * 4) + (hasExtension ? 4 : 0);

    unsigned char nonceBytes[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES] = {0};
    std::memcpy(nonceBytes, packet.constData() + packet.size() - 4, 4);

    int ciphertextLen = packet.size() - headerSize - 4;
    if (ciphertextLen <= 0) return;

    QByteArray plaintext(ciphertextLen, static_cast<char>(0));
    unsigned long long plaintextLen = 0;

    int result = crypto_aead_xchacha20poly1305_ietf_decrypt(
        reinterpret_cast<unsigned char *>(plaintext.data()), &plaintextLen,
        nullptr,
        data + headerSize, ciphertextLen,
        data, headerSize,
        nonceBytes,
        reinterpret_cast<const unsigned char *>(m_secretKey.constData()));

    if (result != 0) {
        m_decryptFailCount++;
        if (m_decryptFailCount % 50 == 1) {
            log(QString("Decrypt failed on incoming packet (count=%1)").arg(m_decryptFailCount));
        }
        return;
    }

    plaintext.resize(static_cast<int>(plaintextLen));

    m_receivedFrameCount++;
    if (m_receivedFrameCount % 50 == 1) {
        log(QString("Successfully decoded %1 incoming audio frames").arg(m_receivedFrameCount));
    }

    m_audio.feedIncomingOpus(plaintext);
}
