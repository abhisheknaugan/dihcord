#pragma once

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>
#include <cstdint>

// Opaque libdave handle types (from includes/dave/dave.h)
extern "C" {
typedef struct DAVESessionHandle_s* DAVESessionHandle;
typedef struct DAVEEncryptorHandle_s* DAVEEncryptorHandle;
typedef struct DAVEDecryptorHandle_s* DAVEDecryptorHandle;
typedef struct DAVEKeyRatchetHandle_s* DAVEKeyRatchetHandle;
}

// Faithful C++ port of the proven DAVE/MLS orchestration sequence (call
// order, opcode numbers, and state machine verified against a real
// working reference implementation - not guessed). Wraps libdave's C API
// (daveSessionCreate, daveEncryptorEncrypt, etc.) with the exact same
// call sequence: session init -> key package exchange -> external
// sender -> proposals/commit/welcome -> epoch transition -> per-user
// key ratchets feeding one shared Encryptor (for our own audio) and one
// Decryptor per remote SSRC (for theirs).
//
// This class knows nothing about WebSockets or UDP - VoiceClient hands
// it raw opcode payloads and asks it to encrypt/decrypt raw Opus frames;
// this class hands back bytes to send or bytes that were decrypted.
class DaveSession : public QObject
{
    Q_OBJECT
public:
    explicit DaveSession(QObject *parent = nullptr);
    ~DaveSession() override;

    // Called when Session Description reports dave_protocol_version > 0.
    void init(uint16_t protocolVersion, uint64_t channelId, const QString &selfUserId);

    void reset();

    // --- Binary voice-gateway MLS opcodes (25/27/29/30), payload only
    // (3-byte [seq][seq][opcode] header already stripped by VoiceClient) ---
    void handleExternalSender(const QByteArray &payload);
    void handleProposals(const QByteArray &payload);
    void handleAnnounceCommit(const QByteArray &payload);
    void handleWelcome(const QByteArray &payload);

    // --- JSON voice-gateway transition opcodes (21/22) ---
    void handlePrepareTransition(int protocolVersion, int transitionId);
    void handleExecuteTransition(int transitionId);

    // --- Speaking (op5) / ssrc<->userId bookkeeping ---
    void addSsrcMapping(uint32_t ssrc, const QString &userId);
    void setLocalSsrc(uint32_t ssrc);
    // Reliable channel-membership tracking, independent of Speaking events
    // (which only fire once someone actually talks - too late for MLS
    // welcome/proposal processing that needs to recognize members from
    // the moment they join).
    void addKnownMember(const QString &userId);

    // --- Media encryption, once a transition has completed ---
    bool isDaveEnabled() const { return m_daveEnabled; }
    bool wasDaveNegotiated() const { return m_daveNegotiated; }
    bool hasUserIdForSsrc(uint32_t ssrc) const { return m_ssrcToUserId.contains(ssrc); }
    // Returns encrypted bytes, or an empty array if passthrough/unavailable
    // (caller should send the frame unencrypted in that case).
    QByteArray encryptOpusFrame(uint32_t ssrc, const QByteArray &plainOpus);
    // Returns decrypted bytes, or an empty array on failure.
    QByteArray decryptOpusFrame(uint32_t ssrc, const QByteArray &cipherOpus);

signals:
    // Raw bytes VoiceClient should send as a binary voice-gateway message
    // with the given opcode (26=key package, 28=commit/welcome).
    void sendBinaryOpcode(int opcode, const QByteArray &payload);
    // JSON {"op": opcode, "d": {"transition_id": N}} VoiceClient should
    // send on the text voice-gateway connection (opcode 23 or 31).
    void sendJsonOpcode(int opcode, int transitionId);
    void log(const QString &line);

private:
    void reinit();
    void completeTransitionIfReady();
    QString userIdForSsrc(uint32_t ssrc) const;
    DAVEDecryptorHandle decryptorForSsrc(uint32_t ssrc, const QString &userId);

    static void mlsFailureCallback(const char *source, const char *reason, void *userData);
    static void logSinkCallback(int severity, const char *file, int line, const char *message);
    static DaveSession *s_activeInstance;

    DAVESessionHandle m_session = nullptr;
    DAVEEncryptorHandle m_encryptor = nullptr;
    QMap<uint32_t, QString> m_ssrcToUserId;
    QSet<QString> m_knownMembers; // everyone actually in the channel, from VOICE_STATE_UPDATE
    QMap<QString, DAVEDecryptorHandle> m_decryptorsByUserId;
    QMap<QString, DAVEKeyRatchetHandle> m_decryptorRatchets; // must stay alive as long as the decryptor uses them
    DAVEKeyRatchetHandle m_encryptorRatchet = nullptr;       // same - encryptor keeps a live reference, doesn't own it
    QMap<uint32_t, QString> m_decryptorSsrcOwner;
    QMap<QString, qint64> m_lastRatchetRefreshMs;
    int m_decryptFailureCount = 0; // rate-limits expensive ratchet refresh per user // which userId's decryptor is bound to this ssrc

    uint16_t m_protocolVersion = 0;
    uint16_t m_pendingProtocolVersion = 0;
    int m_pendingTransitionId = -1;
    bool m_pendingTransitionReady = false;
    bool m_pendingTransitionExecuted = false;
    bool m_daveEnabled = false;
    bool m_daveNegotiated = false;
    bool m_daveDowngraded = false;

    uint64_t m_channelId = 0;
    QString m_selfUserId;
    uint32_t m_localSsrc = 0;
};
