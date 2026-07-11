#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QMainWindow>
#include <QMap>

#include "gatewayclient.h"
#include "processwatcher.h"
#include "profileviewdialog.h"
#include "restclient.h"
#include "serverprofiledialog.h"
#include "voiceclient.h"
#include "voicesettingsdialog.h"

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;

// Phase 3-5 combined: message history + sending, profile bar with a
// status dropdown (live gateway presence), and automatic "Playing X"
// rich presence from ProcessWatcher matched against Discord's own
// detectable-games list.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &token, QWidget *parent = nullptr);

private slots:
    // Guilds / channels (Phase 1-2)
    void onGuildsFetched(const QByteArray &jsonPayload);
    void onGuildsFetchFailed(const QString &reason);
    void onGatewayReady(const QJsonObject &readyPayload);
    void onGuildAvailable(const QJsonObject &guild);
    void onGatewayError(const QString &reason);
    void onGuildSelected(QListWidgetItem *item);
    void onChannelSelected(QListWidgetItem *item);

    // Messages (Phase 3)
    void onMessageReceived(const QJsonObject &message);
    void onMessagesFetched(const QString &channelId, const QJsonArray &messages);
    void onMessagesFetchFailed(const QString &channelId, const QString &reason);
    void onSendClicked();

    // Profile (Phase 4)
    void onCurrentUserFetched(const QJsonObject &user);
    void onEditProfileClicked();
    void onProfileSaveRequested(const QJsonObject &patchFields);
    void onProfileUpdated(const QJsonObject &updatedUser);
    void onProfileUpdateFailed(const QString &reason);
    void onStatusChanged(int index);

    // Rich presence (Phase 5)
    void onDetectableGamesFetched(const QJsonArray &games);
    void onGameDetected(const QString &displayName, const QString &applicationId);
    void onGameEnded();

    // Voice (Phase 6 - signaling only, no audio yet)
    void onVoiceStateUpdate(const QJsonObject &data);
    void onVoiceServerUpdate(const QJsonObject &data);
    void onVoiceHandshakeComplete();
    void onVoiceError(const QString &reason);
    void onVoiceLog(const QString &line);

    void onMuteClicked();
    void onDeafenClicked();
    void onDisconnectVoiceClicked();
    void onVoiceSettingsClicked();

    // Members, friends, DMs, profiles (Phase 7)
    void onMemberClicked(QListWidgetItem *item);
    void onServerInfoClicked();
    void onRelationshipsFetched(const QJsonArray &friends);
    void onDMChannelsFetched(const QJsonArray &channels);
    void onDMOpened(const QJsonObject &channel);
    void onUserProfileFetched(const QJsonObject &user);
    void onUserProfileFetchFailed(const QString &reason);
    void onMessageProfileRequested(const QString &userId);

private:
    void populateChannelsForGuild(const QString &guildId);
    void populateMemberList(const QString &guildId);
    void populateFriendsAndDMs();
    void openDMChannel(const QJsonObject &dmChannel);
    void appendMessageToView(const QJsonObject &message);

    RestClient *m_rest;
    GatewayClient *m_gateway;
    ProcessWatcher *m_processWatcher;
    VoiceClient *m_voiceClient;

    // Top profile bar
    QLabel *m_usernameLabel;
    QLabel *m_playingLabel;
    QComboBox *m_statusCombo;
    QPushButton *m_editProfileButton;

    // Main panes
    QListWidget *m_guildList;
    QListWidget *m_channelList;
    QListWidget *m_messageView;
    QLineEdit *m_messageInput;
    QPushButton *m_sendButton;

    // Voice control bar - only shown while connected to a voice channel
    QWidget *m_voiceBar;
    QLabel *m_voiceStatusLabel;
    QPushButton *m_muteButton;
    QPushButton *m_deafenButton;
    QPushButton *m_disconnectVoiceButton;
    QPushButton *m_voiceSettingsButton;

    // Member list panel (per server) + server info button
    QListWidget *m_memberList;
    QPushButton *m_serverInfoButton;

    QString m_token;
    QString m_selectedChannelId;
    QJsonObject m_currentUser;
    QString m_currentActivityName; // empty if not currently "playing" anything

    QMap<QString, QJsonArray> m_guildChannels;
    QMap<QString, QJsonArray> m_guildVoiceStates;
    QMap<QString, QJsonArray> m_guildMembers;
    QMap<QString, QJsonObject> m_fullGuildData; // for server info popup // guildId -> members array from GUILD_CREATE
    QJsonArray m_friendsList;
    QJsonArray m_dmChannels;
    bool m_viewingFriends = false; // guildId -> voice_states array from GUILD_CREATE/READY

    // Voice join is a two-part async handshake on the main gateway -
    // VOICE_STATE_UPDATE gives us session_id, VOICE_SERVER_UPDATE gives
    // us endpoint+token, and they can arrive in either order. We stash
    // whichever comes first and connect once both are in hand.
    QString m_pendingVoiceGuildId;
    QString m_pendingVoiceChannelId;
    QString m_pendingSessionId;
    QString m_pendingVoiceEndpoint;
    QString m_pendingVoiceToken;
};
