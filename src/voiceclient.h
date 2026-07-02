#pragma once

#include <QHostAddress>
#include <QJsonObject>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

class QWebSocket;

// Handles Discord's voice protocol handshake:
//   1. Connect to the voice gateway WebSocket (separate from the main
//      gateway - different URL, different opcodes).
//   2. Identify with server_id/user_id/session_id/token.
//   3. Receive Ready: ssrc, UDP ip/port, supported encryption modes.
//   4. UDP "IP discovery" packet to learn our own public ip/port as seen
//      by Discord's voice server (needed because of NAT).
//   5. Select Protocol: tell Discord our external ip/port + chosen mode.
//   6. Receive Session Description: the actual secret_key used to
//      encrypt/decrypt audio packets.
//
// PHASE A SCOPE: this class gets us all the way through step 6 and logs
// every step. It deliberately does NOT send or receive any actual audio
// yet - that needs Opus (codec) and libsodium (encryption), two native
// libraries this project doesn't depend on yet. Once this handshake is
// verified working end-to-end, Phase B adds the real audio pipeline on
// top of the secret_key/ssrc/udp socket this class already sets up.
class VoiceClient : public QObject
{
    Q_OBJECT
public:
    explicit VoiceClient(QObject *parent = nullptr);
    ~VoiceClient() override;

    void connectVoice(const QString &endpoint, const QString &token, const QString &guildId,
                       const QString &userId, const QString &sessionId);
    void disconnectVoice();

signals:
    // Emitted once the full handshake completes - secret_key is ready,
    // meaning audio could now be encrypted and sent. Nothing sent yet.
    void voiceHandshakeComplete();
    void voiceError(const QString &reason);
    void voiceLog(const QString &line); // mirrors GatewayClient's file logger

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onTextMessageReceived(const QString &message);
    void onUdpReadyRead();
    void sendHeartbeat();

private:
    enum VoiceOpcode {
        Identify = 0,
        SelectProtocol = 1,
        VReady = 2,
        Heartbeat = 3,
        SessionDescription = 4,
        Speaking = 5,
        VHeartbeatAck = 6,
        Hello = 8,
        Resumed = 9
    };

    void handlePayload(const QJsonObject &payload);
    void sendIdentify();
    void send(const QJsonObject &payload);
    void performIpDiscovery();
    void sendSelectProtocol(const QString &externalIp, quint16 externalPort);
    void log(const QString &line);

    QWebSocket *m_socket;
    QUdpSocket *m_udpSocket;
    QTimer *m_heartbeatTimer;

    QString m_token;
    QString m_guildId;
    QString m_userId;
    QString m_sessionId;

    QHostAddress m_voiceServerIp;
    quint16 m_voiceServerPort = 0;
    quint32 m_ssrc = 0;
    QStringList m_availableModes;

    QByteArray m_secretKey; // filled in once Session Description arrives
    QString m_selectedMode;
    qint64 m_lastVoiceSequence = -1; // "seq" field from incoming voice gateway messages
};
