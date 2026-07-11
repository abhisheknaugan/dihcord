#include "davesession.h"

#include <QDateTime>
#include <dave/dave.h>
#include <cstring>

namespace {
constexpr int OP_KEY_PACKAGE = 26;
constexpr int OP_COMMIT_WELCOME = 28;
constexpr int OP_READY_FOR_TRANSITION = 23;
constexpr int OP_INVALID_COMMIT = 31;
}

DaveSession::DaveSession(QObject *parent) : QObject(parent)
{
    s_activeInstance = this;
    daveSetLogSinkCallback(reinterpret_cast<DAVELogSinkCallback>(&DaveSession::logSinkCallback));
}

DaveSession *DaveSession::s_activeInstance = nullptr;

void DaveSession::mlsFailureCallback(const char *source, const char *reason, void * /*userData*/)
{
    if (s_activeInstance) {
        emit s_activeInstance->log(QString("MLS FAILURE [%1]: %2")
                .arg(source ? source : "?", reason ? reason : "?"));
    }
}

void DaveSession::logSinkCallback(int severity, const char *file, int line, const char *message)
{
    if (s_activeInstance && severity >= 2) { // only warning(2)/error(3+), skip verbose(0)/info(1) per-frame noise
        emit s_activeInstance->log(QString("libdave[%1] %2:%3 - %4")
                .arg(severity).arg(file ? file : "?").arg(line).arg(message ? message : "?"));
    }
}

DaveSession::~DaveSession()
{
    reset();
}

void DaveSession::reset()
{
    if (m_session) daveSessionDestroy(m_session);
    if (m_encryptor) daveEncryptorDestroy(m_encryptor);
    for (auto it = m_decryptorsByUserId.begin(); it != m_decryptorsByUserId.end(); ++it) {
        if (it.value()) daveDecryptorDestroy(it.value());
    }
    for (auto it = m_decryptorRatchets.begin(); it != m_decryptorRatchets.end(); ++it) {
        if (it.value()) daveKeyRatchetDestroy(it.value());
    }
    if (m_encryptorRatchet) daveKeyRatchetDestroy(m_encryptorRatchet);
    m_session = nullptr;
    m_encryptor = nullptr;
    m_encryptorRatchet = nullptr;
    m_decryptorsByUserId.clear();
    m_decryptorRatchets.clear();
    m_decryptorSsrcOwner.clear();
    m_ssrcToUserId.clear();
    m_pendingTransitionId = -1;
    m_pendingTransitionReady = false;
    m_pendingTransitionExecuted = false;
    m_daveEnabled = false;
    m_daveDowngraded = false;
    m_daveNegotiated = false;
    m_protocolVersion = 0;
    m_pendingProtocolVersion = 0;
    m_localSsrc = 0;
}

void DaveSession::init(uint16_t protocolVersion, uint64_t channelId, const QString &selfUserId)
{
    m_protocolVersion = protocolVersion;
    m_pendingProtocolVersion = protocolVersion;
    m_channelId = channelId;
    m_selfUserId = selfUserId;
    m_daveNegotiated = (protocolVersion > 0);
    m_knownMembers.insert(selfUserId);

    if (!m_session) {
        m_session = daveSessionCreate(nullptr, nullptr, &DaveSession::mlsFailureCallback, nullptr);
    }
    reinit();
}

void DaveSession::reinit()
{
    if (!m_session) return;

    m_daveEnabled = false;
    m_daveDowngraded = false;
    m_pendingTransitionReady = false;
    m_pendingTransitionExecuted = false;
    m_pendingTransitionId = -1;

    QByteArray uidBytes = m_selfUserId.toUtf8();
    daveSessionInit(m_session, m_protocolVersion, m_channelId, uidBytes.constData());

    uint8_t *pkg = nullptr;
    size_t pkgSize = 0;
    daveSessionGetMarshalledKeyPackage(m_session, &pkg, &pkgSize);
    if (pkg && pkgSize > 0) {
        emit sendBinaryOpcode(OP_KEY_PACKAGE, QByteArray(reinterpret_cast<const char *>(pkg), static_cast<int>(pkgSize)));
        daveFree(pkg);
    }

    for (auto it = m_decryptorsByUserId.begin(); it != m_decryptorsByUserId.end(); ++it) {
        if (it.value()) daveDecryptorDestroy(it.value());
    }
    for (auto it = m_decryptorRatchets.begin(); it != m_decryptorRatchets.end(); ++it) {
        if (it.value()) daveKeyRatchetDestroy(it.value());
    }
    m_decryptorsByUserId.clear();
    m_decryptorRatchets.clear();
    m_decryptorSsrcOwner.clear();

    if (m_encryptor) daveEncryptorDestroy(m_encryptor);
    if (m_encryptorRatchet) { daveKeyRatchetDestroy(m_encryptorRatchet); m_encryptorRatchet = nullptr; }
    m_encryptor = daveEncryptorCreate();
    if (m_localSsrc) {
        daveEncryptorAssignSsrcToCodec(m_encryptor, m_localSsrc, DAVE_CODEC_OPUS);
    }

    log("DaveSession: reinit complete, key package sent");
}

void DaveSession::handleExternalSender(const QByteArray &payload)
{
    if (!m_session) {
        m_session = daveSessionCreate(nullptr, nullptr, &DaveSession::mlsFailureCallback, nullptr);
    }
    if (m_session) {
        daveSessionSetExternalSender(m_session,
            reinterpret_cast<const uint8_t *>(payload.constData()), payload.size());
        log("DaveSession: external sender set");
    }
}

void DaveSession::handleProposals(const QByteArray &payload)
{
    if (!m_session) return;

    QList<QByteArray> userIdBytes;
    QList<const char *> userPtrs;
    for (const QString &uid : m_knownMembers) {
        userIdBytes.append(uid.toUtf8());
    }
    for (const QByteArray &b : userIdBytes) {
        userPtrs.append(b.constData());
    }

    uint8_t *resp = nullptr;
    size_t respSize = 0;
    daveSessionProcessProposals(m_session,
        reinterpret_cast<const uint8_t *>(payload.constData()), payload.size(),
        userPtrs.isEmpty() ? nullptr : userPtrs.data(), userPtrs.size(),
        &resp, &respSize);

    if (resp && respSize > 0) {
        emit sendBinaryOpcode(OP_COMMIT_WELCOME,
            QByteArray(reinterpret_cast<const char *>(resp), static_cast<int>(respSize)));
        daveFree(resp);
        log("DaveSession: proposals processed, commit/welcome sent");
    }
}

void DaveSession::completeTransitionIfReady()
{
    if (!m_pendingTransitionReady) return;
    m_pendingTransitionReady = false;
    m_pendingTransitionExecuted = false;

    if (!m_session || !m_encryptor) return;

    if (m_localSsrc) {
        daveEncryptorAssignSsrcToCodec(m_encryptor, m_localSsrc, DAVE_CODEC_OPUS);
    }

    QByteArray selfUidBytes = m_selfUserId.toUtf8();
    DAVEKeyRatchetHandle selfRatchet = daveSessionGetKeyRatchet(m_session, selfUidBytes.constData());
    if (!selfRatchet) {
        m_daveEnabled = false;
        m_pendingTransitionId = -1;
        log("DaveSession: transition complete but no self key ratchet - staying disabled");
        return;
    }
    daveEncryptorSetKeyRatchet(m_encryptor, selfRatchet);
    if (m_encryptorRatchet) {
        daveKeyRatchetDestroy(m_encryptorRatchet); // safe to free now - encryptor has transitioned off it
    }
    m_encryptorRatchet = selfRatchet; // keep alive - encryptor holds a live reference, doesn't own it

    for (auto it = m_decryptorsByUserId.begin(); it != m_decryptorsByUserId.end(); ++it) {
        QByteArray uidBytes = it.key().toUtf8();
        DAVEKeyRatchetHandle ratchet = daveSessionGetKeyRatchet(m_session, uidBytes.constData());
        if (ratchet) {
            daveDecryptorTransitionToKeyRatchet(it.value(), ratchet);
            DAVEKeyRatchetHandle old = m_decryptorRatchets.value(it.key(), nullptr);
            if (old) daveKeyRatchetDestroy(old);
            m_decryptorRatchets[it.key()] = ratchet;
        }
    }

    if (!m_daveEnabled) {
        m_daveEnabled = true;
        m_daveDowngraded = false;
    }
    m_pendingTransitionId = -1;
    log("DaveSession: transition complete, DAVE enabled");
}

void DaveSession::handleAnnounceCommit(const QByteArray &payload)
{
    if (!m_session || payload.size() < 2) return;

    int transitionId = (static_cast<unsigned char>(payload[0]) << 8) | static_cast<unsigned char>(payload[1]);
    QByteArray commitData = payload.mid(2);
    m_pendingTransitionId = transitionId;

    DAVECommitResultHandle result = daveSessionProcessCommit(m_session,
        reinterpret_cast<const uint8_t *>(commitData.constData()), commitData.size());

    if (result && !daveCommitResultIsFailed(result) && !daveCommitResultIsIgnored(result)) {
        log(QString("DaveSession: commit processed OK, transition_id=%1").arg(transitionId));
        m_pendingTransitionReady = true;
        emit sendJsonOpcode(OP_READY_FOR_TRANSITION, transitionId);
        if (transitionId == 0 || (m_pendingTransitionExecuted && m_pendingTransitionId == transitionId)) {
            completeTransitionIfReady();
        }
    } else if (result && daveCommitResultIsFailed(result)) {
        log("DaveSession: commit processing FAILED");
        emit sendJsonOpcode(OP_INVALID_COMMIT, transitionId);
    } else {
        log("DaveSession: commit ignored");
    }
    if (result) daveCommitResultDestroy(result);
}

void DaveSession::handleWelcome(const QByteArray &payload)
{
    if (!m_session || payload.size() < 2) return;

    int transitionId = (static_cast<unsigned char>(payload[0]) << 8) | static_cast<unsigned char>(payload[1]);
    QByteArray welcomeData = payload.mid(2);
    m_pendingTransitionId = transitionId;

    QList<QByteArray> userIdBytes;
    QList<const char *> userPtrs;
    for (const QString &uid : m_knownMembers) {
        userIdBytes.append(uid.toUtf8());
    }
    for (const QByteArray &b : userIdBytes) {
        userPtrs.append(b.constData());
    }

    DAVEWelcomeResultHandle result = daveSessionProcessWelcome(m_session,
        reinterpret_cast<const uint8_t *>(welcomeData.constData()), welcomeData.size(),
        userPtrs.isEmpty() ? nullptr : userPtrs.data(), userPtrs.size());

    if (result) {
        log(QString("DaveSession: welcome processed OK, transition_id=%1").arg(transitionId));
        m_pendingTransitionReady = true;
        emit sendJsonOpcode(OP_READY_FOR_TRANSITION, transitionId);
        if (transitionId == 0 || (m_pendingTransitionExecuted && m_pendingTransitionId == transitionId)) {
            completeTransitionIfReady();
        }
        daveWelcomeResultDestroy(result);
    } else {
        log("DaveSession: welcome processing failed");
    }
}

void DaveSession::handlePrepareTransition(int protocolVersion, int transitionId)
{
    m_pendingProtocolVersion = static_cast<uint16_t>(protocolVersion);
    m_pendingTransitionId = transitionId;
    m_pendingTransitionReady = false;
    m_pendingTransitionExecuted = false;

    if (transitionId == 0) {
        m_pendingTransitionReady = true;
        emit sendJsonOpcode(OP_READY_FOR_TRANSITION, 0);
        completeTransitionIfReady();
    } else {
        emit sendJsonOpcode(OP_READY_FOR_TRANSITION, transitionId);
    }
}

void DaveSession::handleExecuteTransition(int transitionId)
{
    m_pendingTransitionId = transitionId;
    m_pendingTransitionExecuted = true;

    if (m_pendingProtocolVersion != m_protocolVersion) {
        m_protocolVersion = m_pendingProtocolVersion;
        if (m_protocolVersion == 0) {
            m_daveEnabled = false;
            m_daveDowngraded = true;
            log("DaveSession: server downgraded us off DAVE");
            return;
        }
    }

    if (m_pendingTransitionReady && m_pendingTransitionId == transitionId) {
        completeTransitionIfReady();
    }
}

void DaveSession::addSsrcMapping(uint32_t ssrc, const QString &userId)
{
    m_ssrcToUserId[ssrc] = userId;
    m_knownMembers.insert(userId);
}

void DaveSession::addKnownMember(const QString &userId)
{
    if (!userId.isEmpty()) {
        m_knownMembers.insert(userId);
    }
}

void DaveSession::setLocalSsrc(uint32_t ssrc)
{
    m_localSsrc = ssrc;
    if (m_encryptor) {
        daveEncryptorAssignSsrcToCodec(m_encryptor, ssrc, DAVE_CODEC_OPUS);
    }
}

QString DaveSession::userIdForSsrc(uint32_t ssrc) const
{
    return m_ssrcToUserId.value(ssrc);
}

DAVEDecryptorHandle DaveSession::decryptorForSsrc(uint32_t ssrc, const QString &userId)
{
    if (userId.isEmpty()) return nullptr;

    if (m_decryptorsByUserId.contains(userId)) {
        return m_decryptorsByUserId.value(userId);
    }

    DAVEDecryptorHandle decryptor = daveDecryptorCreate();
    if (m_session) {
        QByteArray uidBytes = userId.toUtf8();
        DAVEKeyRatchetHandle ratchet = daveSessionGetKeyRatchet(m_session, uidBytes.constData());
        if (ratchet) {
            daveDecryptorTransitionToKeyRatchet(decryptor, ratchet);
            m_decryptorRatchets[userId] = ratchet; // keep alive - decryptor holds a live reference, doesn't own it
        }
    }
    m_decryptorsByUserId[userId] = decryptor;
    m_decryptorSsrcOwner[ssrc] = userId;
    return decryptor;
}

QByteArray DaveSession::encryptOpusFrame(uint32_t ssrc, const QByteArray &plainOpus)
{
    if (!m_daveEnabled || !m_encryptor) return QByteArray();

    size_t maxSize = daveEncryptorGetMaxCiphertextByteSize(m_encryptor, DAVE_MEDIA_TYPE_AUDIO, plainOpus.size());
    QByteArray out(static_cast<int>(maxSize), Qt::Uninitialized);
    size_t written = 0;

    DAVEEncryptorResultCode result = daveEncryptorEncrypt(m_encryptor, DAVE_MEDIA_TYPE_AUDIO, ssrc,
        reinterpret_cast<const uint8_t *>(plainOpus.constData()), plainOpus.size(),
        reinterpret_cast<uint8_t *>(out.data()), out.size(), &written);

    if (result != DAVE_ENCRYPTOR_RESULT_CODE_SUCCESS) {
        emit log(QString("encryptOpusFrame: libdave encrypt failed, result code=%1").arg(static_cast<int>(result)));
        return QByteArray();
    }
    out.resize(static_cast<int>(written));
    return out;
}

QByteArray DaveSession::decryptOpusFrame(uint32_t ssrc, const QByteArray &cipherOpus)
{
    if (!m_daveEnabled) return QByteArray();

    QString userId = userIdForSsrc(ssrc);
    if (userId.isEmpty()) {
        emit log(QString("decryptOpusFrame: no userId known for ssrc=%1 - never got a Speaking event for it").arg(ssrc));
        return QByteArray();
    }

    DAVEDecryptorHandle decryptor = decryptorForSsrc(ssrc, userId);
    if (!decryptor) {
        emit log(QString("decryptOpusFrame: failed to create/find decryptor for user %1").arg(userId));
        return QByteArray();
    }

    size_t maxSize = daveDecryptorGetMaxPlaintextByteSize(decryptor, DAVE_MEDIA_TYPE_AUDIO, cipherOpus.size());
    QByteArray out(static_cast<int>(maxSize), Qt::Uninitialized);
    size_t written = 0;

    DAVEDecryptorResultCode result = daveDecryptorDecrypt(decryptor, DAVE_MEDIA_TYPE_AUDIO,
        reinterpret_cast<const uint8_t *>(cipherOpus.constData()), cipherOpus.size(),
        reinterpret_cast<uint8_t *>(out.data()), out.size(), &written);

    if (result != DAVE_DECRYPTOR_RESULT_CODE_SUCCESS) {
        m_decryptFailureCount++;
        if (m_decryptFailureCount % 100 == 1) {
            emit log(QString("decryptOpusFrame: libdave decrypt failed for user %1, result code=%2, total failures=%3")
                    .arg(userId).arg(static_cast<int>(result)).arg(m_decryptFailureCount));
        }
        return QByteArray();
    }
    out.resize(static_cast<int>(written));
    return out;
}
