#pragma once

#include <QFile>
#include <QJsonObject>
#include <QObject>
#include <QTextStream>
#include <QTimer>

class QWebSocket;

// Discord Gateway v10 client. WebSocket only (QtWebSockets - CPU-only,
// no GPU involvement). Handles just enough of the protocol for Phase 2:
// Hello -> Identify -> heartbeat loop -> Ready / Guild Create / Message
// Create dispatch events. Resume-on-reconnect and full opcode coverage
// (Voice State Update, Presence Update, etc.) come in later phases.
class GatewayClient : public QObject
{
    Q_OBJECT
public:
    explicit GatewayClient(QObject *parent = nullptr);
    ~GatewayClient() override;

    void connectGateway(const QString &token);
    void disconnectGateway();

    // Status: "online" | "idle" | "dnd" | "invisible"
    // activityName/activityAppId empty = no activity shown ("stop playing").
    void updatePresence(const QString &status, const QString &activityName = QString(),
                        const QString &activityAppId = QString());

    // Discord lazy-loads guild data (channels, members) for accounts in
    // many servers - a guild sits "unavailable" until the client asks for
    // it specifically. This is opcode 14, undocumented but required for
    // user-token clients with more than a handful of guilds. Call this
    // when the user actually clicks into a server.
    void requestGuildSubscription(const QString &guildId);

    // Voice channel join/leave - sends opcode 4 (Voice State Update) on
    // the main gateway. Discord responds asynchronously with
    // VOICE_STATE_UPDATE (confirms our session) and VOICE_SERVER_UPDATE
    // (gives us the actual voice server to connect to) - see the two
    // signals below.
    void joinVoiceChannel(const QString &guildId, const QString &channelId);
    void leaveVoiceChannel(const QString &guildId);

signals:
    void connected();
    void ready(const QJsonObject &readyPayload);
    void guildAvailable(const QJsonObject &guild);      // one per GUILD_CREATE
    void messageReceived(const QJsonObject &message);   // one per MESSAGE_CREATE
    void gatewayError(const QString &reason);
    void gatewayClosed();

    // Raw passthrough - caller filters by user_id/guild_id as needed.
    void voiceStateUpdate(const QJsonObject &data);   // dispatch: VOICE_STATE_UPDATE
    void voiceServerUpdate(const QJsonObject &data);  // dispatch: VOICE_SERVER_UPDATE

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onTextMessageReceived(const QString &message);
    void sendHeartbeat();

private:
    // Gateway opcodes we handle.
    enum Opcode {
        Dispatch = 0,
        Heartbeat = 1,
        Identify = 2,
        PresenceUpdate = 3,
        Reconnect = 7,
        InvalidSession = 9,
        Hello = 10,
        HeartbeatAck = 11,
        GuildSubscriptions = 14,
        VoiceStateUpdateOp = 4
    };

    void handlePayload(const QJsonObject &payload);
    void sendIdentify();
    void send(const QJsonObject &payload);
    void log(const QString &line); // writes to a debug log file next to the exe

    QWebSocket *m_socket;
    QTimer *m_heartbeatTimer;
    QString m_token;

    qint64 m_lastSequence = -1;      // "s" field, needed for heartbeats + resume
    bool m_heartbeatAcked = true;    // simple zombie-connection guard
};
