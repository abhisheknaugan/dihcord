#pragma once

#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>

class QNetworkAccessManager;
class QNetworkReply;

// Thin wrapper around Discord's REST API v10. Deliberately built on
// QNetworkAccessManager (CPU-only, no GPU involvement) rather than any
// embedded browser component.
class RestClient : public QObject
{
    Q_OBJECT
public:
    explicit RestClient(QObject *parent = nullptr);

    // --- Auth (Phase 1) ---
    void login(const QString &email, const QString &password);
    void submitMfaCode(const QString &ticket, const QString &code);

    // --- Guilds / channels (Phase 1-2) ---
    void fetchGuilds(const QString &token);

    // --- Messages (Phase 3) ---
    void fetchMessages(const QString &token, const QString &channelId, int limit = 50);
    void sendMessage(const QString &token, const QString &channelId, const QString &content);

    // --- Profile (Phase 4) ---
    void fetchCurrentUser(const QString &token);
    // `patchFields` is merged directly into the PATCH /users/@me body.
    // Supported keys: username, bio, avatar (data: URI string), password
    // (current password, required by Discord when changing username/email).
    void updateProfile(const QString &token, const QJsonObject &patchFields);

    // --- Rich presence support (Phase 5) ---
    // Public endpoint listing every game Discord can auto-detect, with the
    // executable names to match against. No auth required.
    void fetchDetectableGames();

    // --- Friends & DMs (Phase 6) ---
    void fetchRelationships(const QString &token); // friends list
    void fetchDMChannels(const QString &token);
    void createOrOpenDM(const QString &token, const QString &recipientUserId);
    void fetchUserById(const QString &token, const QString &userId);

    // --- Message actions (Phase 8) ---
    void sendMessageWithReply(const QString &token, const QString &channelId, const QString &content,
                               const QString &replyToMessageId);
    void editMessage(const QString &token, const QString &channelId, const QString &messageId,
                      const QString &newContent);
    void deleteMessage(const QString &token, const QString &channelId, const QString &messageId);
    void fetchPinnedMessages(const QString &token, const QString &channelId);
    void sendTypingIndicator(const QString &token, const QString &channelId);
    void ackMessage(const QString &token, const QString &channelId, const QString &messageId);

    // --- Invites & friends (Phase 8) ---
    void joinGuildViaInvite(const QString &token, const QString &inviteCode);
    void addFriendByUsername(const QString &token, const QString &username);
    void respondToFriendRequest(const QString &token, const QString &userId, bool accept);
    void removeFriend(const QString &token, const QString &userId);
    void blockUser(const QString &token, const QString &userId); // for profile popups

signals:
    void loginSucceeded(const QString &token);
    void mfaRequired(const QString &ticket);
    void captchaRequired();
    void loginFailed(const QString &reason);

    void guildsFetched(const QByteArray &jsonPayload);
    void guildsFetchFailed(const QString &reason);

    void messagesFetched(const QString &channelId, const QJsonArray &messages);
    void messagesFetchFailed(const QString &channelId, const QString &reason);
    void messageSendFailed(const QString &channelId, const QString &reason);

    void currentUserFetched(const QJsonObject &user);
    void currentUserFetchFailed(const QString &reason);
    void profileUpdated(const QJsonObject &updatedUser);
    void profileUpdateFailed(const QString &reason);

    void detectableGamesFetched(const QJsonArray &games);
    void detectableGamesFetchFailed(const QString &reason);

    void relationshipsFetched(const QJsonArray &friends);
    void dmChannelsFetched(const QJsonArray &channels);
    void dmOpened(const QJsonObject &channel);
    void userProfileFetched(const QJsonObject &user);
    void userProfileFetchFailed(const QString &reason);
    void pinnedMessagesFetched(const QJsonArray &messages);

private:
    void handleLoginReply(QNetworkReply *reply);

    QNetworkAccessManager *m_net;
    static constexpr const char *kApiBase = "https://discord.com/api/v10";
};
