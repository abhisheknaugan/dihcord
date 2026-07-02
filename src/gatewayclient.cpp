#include "gatewayclient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
#include <QWebSocket>

namespace {
constexpr const char *kGatewayUrl = "wss://gateway.discord.gg/?v=10&encoding=json";
}

void GatewayClient::log(const QString &line)
{
    QFile f("gateway-debug.log");
    f.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);
    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "  " << line << "\n";
}

GatewayClient::GatewayClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QWebSocket())
    , m_heartbeatTimer(new QTimer(this))
{
    connect(m_socket, &QWebSocket::connected, this, &GatewayClient::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &GatewayClient::onSocketDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &GatewayClient::onTextMessageReceived);
    connect(m_socket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        log("Socket error: " + m_socket->errorString());
        emit gatewayError(m_socket->errorString());
    });

    connect(m_heartbeatTimer, &QTimer::timeout, this, &GatewayClient::sendHeartbeat);
}

GatewayClient::~GatewayClient()
{
    disconnectGateway();
    m_socket->deleteLater();
}

void GatewayClient::connectGateway(const QString &token)
{
    log("connectGateway() called");
    m_token = token;
    m_socket->open(QUrl(kGatewayUrl));
}

void GatewayClient::disconnectGateway()
{
    m_heartbeatTimer->stop();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->close();
    }
}

void GatewayClient::onSocketConnected()
{
    log("WebSocket connected (waiting for Hello)");
    emit connected();
}

void GatewayClient::onSocketDisconnected()
{
    log("WebSocket disconnected");
    m_heartbeatTimer->stop();
    emit gatewayClosed();
}

void GatewayClient::onTextMessageReceived(const QString &message)
{
    QJsonObject payload = QJsonDocument::fromJson(message.toUtf8()).object();
    handlePayload(payload);
}

void GatewayClient::handlePayload(const QJsonObject &payload)
{
    int op = payload.value("op").toInt(-1);

    if (payload.contains("s") && !payload.value("s").isNull()) {
        m_lastSequence = payload.value("s").toVariant().toLongLong();
    }

    switch (op) {
    case Hello: {
        int intervalMs = payload.value("d").toObject().value("heartbeat_interval").toInt();
        log(QString("Received Hello, heartbeat_interval=%1ms, sending Identify").arg(intervalMs));
        m_heartbeatTimer->start(intervalMs);
        sendIdentify();
        break;
    }
    case HeartbeatAck: {
        m_heartbeatAcked = true;
        break;
    }
    case Reconnect:
    case InvalidSession: {
        log(QString("Received op %1 (Reconnect/InvalidSession) - re-identifying").arg(op));
        emit gatewayError("Gateway requested reconnect/invalid session (resume not yet implemented)");
        disconnectGateway();
        connectGateway(m_token);
        break;
    }
    case Dispatch: {
        QString eventType = payload.value("t").toString();
        QJsonObject data = payload.value("d").toObject();

        if (eventType == "READY") {
            QJsonArray guilds = data.value("guilds").toArray();
            log(QString("READY received - %1 guilds in payload").arg(guilds.size()));
            emit ready(data);

            // For user accounts, Discord embeds each guild's full data
            // (including its channel list) directly inside READY itself -
            // it does NOT send separate GUILD_CREATE events afterward for
            // guilds that are already available. Only guilds marked
            // unavailable (very large ones) need the lazy-subscribe
            // fallback via requestGuildSubscription(), which still applies
            // for those and will arrive as a genuine GUILD_CREATE later.
            int availableCount = 0;
            for (const QJsonValue &v : guilds) {
                QJsonObject guild = v.toObject();
                bool unavailable = guild.value("unavailable").toBool();
                int channelCount = guild.value("channels").toArray().size();
                log(QString("  guild %1 - unavailable=%2, channels=%3")
                        .arg(guild.value("id").toString())
                        .arg(unavailable ? "true" : "false")
                        .arg(channelCount));
                if (!unavailable) {
                    availableCount++;
                    emit guildAvailable(guild);
                }
            }
            log(QString("%1 of %2 guilds were immediately available from READY")
                    .arg(availableCount).arg(guilds.size()));
        } else if (eventType == "GUILD_CREATE") {
            QString guildId = data.value("id").toString();
            QString guildName = data.value("name").toString();
            int channelCount = data.value("channels").toArray().size();
            bool unavailable = data.value("unavailable").toBool();
            log(QString("GUILD_CREATE: %1 (%2) - %3 channels, unavailable=%4")
                    .arg(guildName, guildId).arg(channelCount).arg(unavailable ? "true" : "false"));
            emit guildAvailable(data);
        } else if (eventType == "MESSAGE_CREATE") {
            emit messageReceived(data);
        } else if (eventType == "VOICE_STATE_UPDATE") {
            log("Dispatch: VOICE_STATE_UPDATE (session_id=" + data.value("session_id").toString() + ")");
            emit voiceStateUpdate(data);
        } else if (eventType == "VOICE_SERVER_UPDATE") {
            log("Dispatch: VOICE_SERVER_UPDATE (endpoint=" + data.value("endpoint").toString() + ")");
            emit voiceServerUpdate(data);
        } else if (!eventType.isEmpty()) {
            log("Dispatch event (ignored): " + eventType);
        }
        break;
    }
    default:
        log(QString("Unhandled opcode: %1").arg(op));
        break;
    }
}

void GatewayClient::sendIdentify()
{
    QJsonObject properties{
        {"os", "Windows 10"},
        {"browser", "Chrome"},
        {"device", ""},
        {"system_locale", "en-US"},
        {"browser_version", "125.0.0.0"},
        {"os_version", "10"},
        {"referrer", ""},
        {"referring_domain", ""},
        {"referrer_current", ""},
        {"referring_domain_current", ""},
        {"release_channel", "stable"},
        {"client_build_number", 300000},
        {"client_event_source", QJsonValue::Null}
    };

    QJsonObject data{
        {"token", m_token},
        {"capabilities", 16381},
        {"properties", properties},
        {"compress", false}
    };

    log("Sending Identify (with capabilities flag)");
    send(QJsonObject{
        {"op", Identify},
        {"d", data}
    });
}

void GatewayClient::sendHeartbeat()
{
    if (!m_heartbeatAcked) {
        // Previous heartbeat never got an ACK - connection is likely dead.
        emit gatewayError("Heartbeat not acknowledged; reconnecting");
        disconnectGateway();
        connectGateway(m_token);
        return;
    }

    m_heartbeatAcked = false;

    QJsonValue seq = (m_lastSequence >= 0) ? QJsonValue(m_lastSequence) : QJsonValue();
    send(QJsonObject{
        {"op", Heartbeat},
        {"d", seq}
    });
}

void GatewayClient::send(const QJsonObject &payload)
{
    m_socket->sendTextMessage(QJsonDocument(payload).toJson(QJsonDocument::Compact));
}

void GatewayClient::updatePresence(const QString &status, const QString &activityName,
                                    const QString &activityAppId)
{
    QJsonArray activities;
    if (!activityName.isEmpty()) {
        QJsonObject activity{
            {"name", activityName},
            {"type", 0} // 0 = "Playing"
        };
        if (!activityAppId.isEmpty()) {
            activity["application_id"] = activityAppId;
        }
        activities.append(activity);
    }

    QJsonObject data{
        {"since", QJsonValue::Null},
        {"activities", activities},
        {"status", status},
        {"afk", false}
    };

    send(QJsonObject{
        {"op", PresenceUpdate},
        {"d", data}
    });
}

void GatewayClient::requestGuildSubscription(const QString &guildId)
{
    log("Sending GuildSubscriptions (opcode 14) for guild " + guildId);
    // Mirrors what the real client sends the moment you click into a
    // server - asks Discord to actually push this guild's full data
    // (channels, member list, typing/activity events) instead of leaving
    // it in the lazy "unavailable" state it starts in.
    QJsonObject data{
        {"guild_id", guildId},
        {"typing", true},
        {"threads", false},
        {"activities", true},
        {"members", QJsonArray()}
    };

    send(QJsonObject{
        {"op", GuildSubscriptions},
        {"d", data}
    });
}

void GatewayClient::joinVoiceChannel(const QString &guildId, const QString &channelId)
{
    log(QString("Sending Voice State Update - join guild=%1 channel=%2").arg(guildId, channelId));
    QJsonObject data{
        {"guild_id", guildId},
        {"channel_id", channelId},
        {"self_mute", false},
        {"self_deaf", false}
    };

    send(QJsonObject{
        {"op", VoiceStateUpdateOp},
        {"d", data}
    });
}

void GatewayClient::leaveVoiceChannel(const QString &guildId)
{
    log("Sending Voice State Update - leave guild=" + guildId);
    QJsonObject data{
        {"guild_id", guildId},
        {"channel_id", QJsonValue::Null},
        {"self_mute", false},
        {"self_deaf", false}
    };

    send(QJsonObject{
        {"op", VoiceStateUpdateOp},
        {"d", data}
    });
}
