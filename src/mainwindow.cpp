#include "mainwindow.h"
#include "profiledialog.h"

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(const QString &token, QWidget *parent)
    : QMainWindow(parent)
    , m_rest(new RestClient(this))
    , m_gateway(new GatewayClient(this))
    , m_processWatcher(new ProcessWatcher(this))
    , m_voiceClient(new VoiceClient(this))
    , m_token(token)
{
    setWindowTitle("RipcordAlt");
    resize(900, 560);

    auto *central = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(6, 6, 6, 6);

    // --- Profile bar (top strip) ---
    auto *profileBar = new QHBoxLayout();
    m_usernameLabel = new QLabel("Loading profile...");
    m_playingLabel = new QLabel();
    m_playingLabel->setStyleSheet("color: gray; font-style: italic;");

    m_statusCombo = new QComboBox();
    m_statusCombo->addItem("Online", "online");
    m_statusCombo->addItem("Idle", "idle");
    m_statusCombo->addItem("Do Not Disturb", "dnd");
    m_statusCombo->addItem("Invisible", "invisible");

    m_editProfileButton = new QPushButton("Edit profile");
    m_serverInfoButton = new QPushButton("Server Info");
    m_serverInfoButton->setEnabled(false);

    profileBar->addWidget(m_usernameLabel);
    profileBar->addWidget(m_playingLabel);
    profileBar->addStretch();
    profileBar->addWidget(m_statusCombo);
    profileBar->addWidget(m_serverInfoButton);
    profileBar->addWidget(m_editProfileButton);
    rootLayout->addLayout(profileBar);

    connect(m_statusCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onStatusChanged);
    connect(m_editProfileButton, &QPushButton::clicked, this, &MainWindow::onEditProfileClicked);
    connect(m_serverInfoButton, &QPushButton::clicked, this, &MainWindow::onServerInfoClicked);

    // --- Main three-pane splitter ---
    auto *splitter = new QSplitter();
    m_guildList = new QListWidget();
    m_channelList = new QListWidget();

    auto *messagePane = new QWidget();
    auto *messagePaneLayout = new QVBoxLayout(messagePane);
    messagePaneLayout->setContentsMargins(0, 0, 0, 0);
    m_messageView = new QListWidget();
    m_messageView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_messageView, &QListWidget::customContextMenuRequested, this, &MainWindow::onMessageContextMenu);
    messagePaneLayout->addWidget(m_messageView);

    auto *inputRow = new QHBoxLayout();
    m_messageInput = new QLineEdit();
    m_messageInput->setPlaceholderText("Select a channel to start typing...");
    m_messageInput->setEnabled(false);
    m_sendButton = new QPushButton("Send");
    m_sendButton->setEnabled(false);
    inputRow->addWidget(m_messageInput);
    inputRow->addWidget(m_sendButton);
    messagePaneLayout->addLayout(inputRow);

    splitter->addWidget(m_guildList);
    splitter->addWidget(m_channelList);
    splitter->addWidget(messagePane);
    splitter->setStretchFactor(2, 1);

    m_memberList = new QListWidget();
    splitter->addWidget(m_memberList);
    connect(m_memberList, &QListWidget::itemClicked, this, &MainWindow::onMemberClicked);

    rootLayout->addWidget(splitter);

    // --- Voice control bar (hidden until actually connected to voice) ---
    m_voiceBar = new QWidget();
    auto *voiceBarLayout = new QHBoxLayout(m_voiceBar);
    voiceBarLayout->setContentsMargins(4, 4, 4, 4);

    m_voiceStatusLabel = new QLabel("Connected to voice");
    m_muteButton = new QPushButton("Mute");
    m_muteButton->setCheckable(true);
    m_deafenButton = new QPushButton("Deafen");
    m_deafenButton->setCheckable(true);
    m_voiceSettingsButton = new QPushButton("Voice Settings");
    m_disconnectVoiceButton = new QPushButton("Disconnect");

    voiceBarLayout->addWidget(m_voiceStatusLabel);
    voiceBarLayout->addStretch();
    voiceBarLayout->addWidget(m_muteButton);
    voiceBarLayout->addWidget(m_deafenButton);
    voiceBarLayout->addWidget(m_voiceSettingsButton);
    voiceBarLayout->addWidget(m_disconnectVoiceButton);

    m_voiceBar->hide();
    rootLayout->addWidget(m_voiceBar);

    connect(m_muteButton, &QPushButton::clicked, this, &MainWindow::onMuteClicked);
    connect(m_deafenButton, &QPushButton::clicked, this, &MainWindow::onDeafenClicked);
    connect(m_disconnectVoiceButton, &QPushButton::clicked, this, &MainWindow::onDisconnectVoiceClicked);
    connect(m_voiceSettingsButton, &QPushButton::clicked, this, &MainWindow::onVoiceSettingsClicked);

    setCentralWidget(central);

    connect(m_guildList, &QListWidget::itemClicked, this, &MainWindow::onGuildSelected);
    connect(m_channelList, &QListWidget::itemClicked, this, &MainWindow::onChannelSelected);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);

    // --- REST wiring ---
    connect(m_rest, &RestClient::guildsFetched, this, &MainWindow::onGuildsFetched);
    connect(m_rest, &RestClient::guildsFetchFailed, this, &MainWindow::onGuildsFetchFailed);
    connect(m_rest, &RestClient::messagesFetched, this, &MainWindow::onMessagesFetched);
    connect(m_rest, &RestClient::messagesFetchFailed, this, &MainWindow::onMessagesFetchFailed);
    connect(m_rest, &RestClient::currentUserFetched, this, &MainWindow::onCurrentUserFetched);
    connect(m_rest, &RestClient::profileUpdated, this, &MainWindow::onProfileUpdated);
    connect(m_rest, &RestClient::profileUpdateFailed, this, &MainWindow::onProfileUpdateFailed);
    connect(m_rest, &RestClient::detectableGamesFetched, this, &MainWindow::onDetectableGamesFetched);

    m_rest->fetchGuilds(m_token);
    m_rest->fetchCurrentUser(m_token);
    m_rest->fetchDetectableGames();

    connect(m_rest, &RestClient::relationshipsFetched, this, &MainWindow::onRelationshipsFetched);
    connect(m_rest, &RestClient::dmChannelsFetched, this, &MainWindow::onDMChannelsFetched);
    connect(m_rest, &RestClient::dmOpened, this, &MainWindow::onDMOpened);
    connect(m_rest, &RestClient::userProfileFetched, this, &MainWindow::onUserProfileFetched);
    connect(m_rest, &RestClient::userProfileFetchFailed, this, &MainWindow::onUserProfileFetchFailed);
    m_rest->fetchRelationships(m_token);
    m_rest->fetchDMChannels(m_token);

    // --- Gateway wiring ---
    connect(m_gateway, &GatewayClient::ready, this, &MainWindow::onGatewayReady);
    connect(m_gateway, &GatewayClient::guildAvailable, this, &MainWindow::onGuildAvailable);
    connect(m_gateway, &GatewayClient::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_gateway, &GatewayClient::gatewayError, this, &MainWindow::onGatewayError);
    m_gateway->connectGateway(m_token);

    // --- Rich presence wiring ---
    connect(m_processWatcher, &ProcessWatcher::gameDetected, this, &MainWindow::onGameDetected);
    connect(m_processWatcher, &ProcessWatcher::gameEnded, this, &MainWindow::onGameEnded);

    // --- Voice wiring (signaling only, Phase A) ---
    connect(m_gateway, &GatewayClient::voiceStateUpdate, this, &MainWindow::onVoiceStateUpdate);
    connect(m_gateway, &GatewayClient::voiceServerUpdate, this, &MainWindow::onVoiceServerUpdate);
    connect(m_voiceClient, &VoiceClient::voiceHandshakeComplete, this, &MainWindow::onVoiceHandshakeComplete);
    connect(m_voiceClient, &VoiceClient::voiceError, this, &MainWindow::onVoiceError);
    connect(m_voiceClient, &VoiceClient::voiceLog, this, &MainWindow::onVoiceLog);
    connect(m_voiceClient, &VoiceClient::voiceDisconnected, this, [this]() {
        m_voiceBar->hide();
        m_muteButton->setChecked(false);
        m_muteButton->setText("Mute");
        m_deafenButton->setChecked(false);
        m_deafenButton->setText("Deafen");
    });
}

// ---------------- Guilds / channels ----------------

void MainWindow::onGuildsFetched(const QByteArray &jsonPayload)
{
    QJsonArray guilds = QJsonDocument::fromJson(jsonPayload).array();
    m_guildList->clear();

    auto *friendsItem = new QListWidgetItem(QString::fromUtf8("\xF0\x9F\x91\xA5 Friends & DMs"));
    friendsItem->setData(Qt::UserRole, "@friends");
    m_guildList->addItem(friendsItem);

    for (const QJsonValue &v : guilds) {
        QJsonObject g = v.toObject();
        auto *item = new QListWidgetItem(g.value("name").toString());
        item->setData(Qt::UserRole, g.value("id").toString());
        m_guildList->addItem(item);
    }
}

void MainWindow::onGuildsFetchFailed(const QString &reason)
{
    QMessageBox::warning(this, "Failed to load servers", reason);
}

void MainWindow::onGatewayReady(const QJsonObject &readyPayload)
{
    Q_UNUSED(readyPayload);
    statusBar()->showMessage("Gateway connected and identified", 3000);
}

void MainWindow::onGuildAvailable(const QJsonObject &guild)
{
    QString guildId = guild.value("id").toString();
    m_guildChannels[guildId] = guild.value("channels").toArray();
    m_guildVoiceStates[guildId] = guild.value("voice_states").toArray();
    m_guildMembers[guildId] = guild.value("members").toArray();
    m_fullGuildData[guildId] = guild;

    QListWidgetItem *current = m_guildList->currentItem();
    if (current && current->data(Qt::UserRole).toString() == guildId) {
        populateChannelsForGuild(guildId);
        populateMemberList(guildId);
    }
}

void MainWindow::onGatewayError(const QString &reason)
{
    statusBar()->showMessage("Gateway: " + reason, 5000);
}

void MainWindow::onGuildSelected(QListWidgetItem *item)
{
    QString guildId = item->data(Qt::UserRole).toString();

    if (guildId == "@friends") {
        m_viewingFriends = true;
        m_serverInfoButton->setEnabled(false);
        m_memberList->clear();
        populateFriendsAndDMs();
        return;
    }

    m_viewingFriends = false;
    m_serverInfoButton->setEnabled(true);
    // Ask Discord to actually push this guild's data - see
    // GatewayClient::requestGuildSubscription for why this is needed.
    m_gateway->requestGuildSubscription(guildId);
    populateChannelsForGuild(guildId);
    populateMemberList(guildId);
}

void MainWindow::populateChannelsForGuild(const QString &guildId)
{
    m_channelList->clear();
    m_messageView->clear();
    m_selectedChannelId.clear();
    m_messageInput->setEnabled(false);
    m_sendButton->setEnabled(false);

    if (!m_guildChannels.contains(guildId)) {
        m_channelList->addItem("(waiting for gateway data...)");
        return;
    }

    const QJsonArray channels = m_guildChannels.value(guildId);
    for (const QJsonValue &v : channels) {
        QJsonObject ch = v.toObject();
        int type = ch.value("type").toInt();

        if (type == 0) { // GUILD_TEXT
            auto *item = new QListWidgetItem("#" + ch.value("name").toString());
            item->setData(Qt::UserRole, ch.value("id").toString());
            item->setData(Qt::UserRole + 1, "text");
            m_channelList->addItem(item);
        } else if (type == 2) { // GUILD_VOICE
            auto *item = new QListWidgetItem(QString::fromUtf8("\xF0\x9F\x94\x8A ") + ch.value("name").toString());
            item->setData(Qt::UserRole, ch.value("id").toString());
            item->setData(Qt::UserRole + 1, "voice");
            m_channelList->addItem(item);
        }
        // Category channels (type 4) and others stay filtered out.
    }
}

void MainWindow::onChannelSelected(QListWidgetItem *item)
{
    QString channelId = item->data(Qt::UserRole).toString();
    if (channelId.isEmpty()) {
        return; // clicked the "(waiting for gateway data...)" placeholder, not a real channel
    }

    QString channelType = item->data(Qt::UserRole + 1).toString();

    if (channelType == "friend") {
        m_rest->createOrOpenDM(m_token, channelId); // channelId here actually holds the user id
        return;
    }

    if (channelType == "voice") {
        QListWidgetItem *guildItem = m_guildList->currentItem();
        if (!guildItem) {
            return;
        }
        QString guildId = guildItem->data(Qt::UserRole).toString();

        m_pendingVoiceGuildId = guildId;
        m_pendingVoiceChannelId = channelId;
        m_pendingSessionId.clear();
        m_pendingVoiceEndpoint.clear();
        m_pendingVoiceToken.clear();

        // Seed everyone already sitting in this channel before we even
        // join - VOICE_STATE_UPDATE only tells us about future changes,
        // not who's already there.
        for (const QJsonValue &v : m_guildVoiceStates.value(guildId)) {
            QJsonObject vs = v.toObject();
            if (vs.value("channel_id").toString() == channelId) {
                QString memberUserId = vs.value("user_id").toString();
                if (!memberUserId.isEmpty()) {
                    m_voiceClient->addKnownMember(memberUserId);
                }
            }
        }

        statusBar()->showMessage("Joining voice channel...", 5000);
        m_gateway->joinVoiceChannel(guildId, channelId);
        return;
    }

    m_selectedChannelId = channelId;
    m_messageView->clear();
    m_messageView->addItem("Loading messages...");
    m_messageInput->setEnabled(true);
    m_sendButton->setEnabled(true);
    m_messageInput->setFocus();

    m_rest->fetchMessages(m_token, m_selectedChannelId);
}

// ---------------- Messages ----------------

void MainWindow::appendMessageToView(const QJsonObject &message)
{
    QJsonObject authorObj = message.value("author").toObject();
    QString author = authorObj.value("username").toString();
    QString content = message.value("content").toString();
    QString displayContent = content;
    if (displayContent.isEmpty()) {
        displayContent = "(no text content - attachment/embed only, not rendered in this phase)";
    }

    auto *item = new QListWidgetItem(QString("%1: %2").arg(author, displayContent));
    item->setData(Qt::UserRole, message.value("id").toString());
    item->setData(Qt::UserRole + 1, authorObj.value("id").toString());
    item->setData(Qt::UserRole + 2, content); // raw content, for editing/copying without the "author: " prefix
    m_messageView->addItem(item);
}

void MainWindow::onMessagesFetched(const QString &channelId, const QJsonArray &messages)
{
    if (channelId != m_selectedChannelId) {
        return; // user already switched channels before this response arrived
    }

    m_messageView->clear();
    // REST returns newest-first; reverse so it reads top-to-bottom oldest-first.
    for (int i = messages.size() - 1; i >= 0; --i) {
        appendMessageToView(messages.at(i).toObject());
    }
    m_messageView->scrollToBottom();
}

void MainWindow::onMessagesFetchFailed(const QString &channelId, const QString &reason)
{
    if (channelId == m_selectedChannelId) {
        m_messageView->clear();
        m_messageView->addItem("Failed to load messages: " + reason);
    }
}

void MainWindow::onMessageReceived(const QJsonObject &message)
{
    if (message.value("channel_id").toString() != m_selectedChannelId) {
        return;
    }
    appendMessageToView(message);
    m_messageView->scrollToBottom();
}

void MainWindow::onSendClicked()
{
    QString content = m_messageInput->text().trimmed();
    if (content.isEmpty() || m_selectedChannelId.isEmpty()) {
        return;
    }
    m_rest->sendMessage(m_token, m_selectedChannelId, content);
    m_messageInput->clear();
    // The sent message will appear via the gateway's MESSAGE_CREATE echo,
    // same path as anyone else's messages - no need to render it here too.
}

// ---------------- Profile ----------------

void MainWindow::onCurrentUserFetched(const QJsonObject &user)
{
    m_currentUser = user;
    m_usernameLabel->setText(user.value("username").toString());
}

void MainWindow::onEditProfileClicked()
{
    auto *dialog = new ProfileDialog(m_currentUser, this);
    connect(dialog, &ProfileDialog::saveRequested, this, &MainWindow::onProfileSaveRequested);
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::onProfileSaveRequested(const QJsonObject &patchFields)
{
    m_rest->updateProfile(m_token, patchFields);
}

void MainWindow::onProfileUpdated(const QJsonObject &updatedUser)
{
    m_currentUser = updatedUser;
    m_usernameLabel->setText(updatedUser.value("username").toString());
    statusBar()->showMessage("Profile updated", 3000);
}

void MainWindow::onProfileUpdateFailed(const QString &reason)
{
    QMessageBox::warning(this, "Profile update failed", reason);
}

void MainWindow::onStatusChanged(int index)
{
    QString status = m_statusCombo->itemData(index).toString();
    m_gateway->updatePresence(status, m_currentActivityName);
}

// ---------------- Rich presence ----------------

void MainWindow::onDetectableGamesFetched(const QJsonArray &games)
{
    QMap<QString, ProcessWatcher::GameInfo> lookup;

    for (const QJsonValue &v : games) {
        QJsonObject game = v.toObject();
        QString appId = game.value("id").toString();
        QString name = game.value("name").toString();

        for (const QJsonValue &execVal : game.value("executables").toArray()) {
            QJsonObject exec = execVal.toObject();
            // Discord stores Windows executable paths like "myapp.exe" or
            // with a leading path fragment - normalize to just the filename.
            QString exeName = exec.value("name").toString().toLower();
            int slash = exeName.lastIndexOf('/');
            if (slash >= 0) {
                exeName = exeName.mid(slash + 1);
            }
            if (!exeName.isEmpty()) {
                lookup.insert(exeName, {appId, name});
            }
        }
    }

    m_processWatcher->setDetectableGames(lookup);
    m_processWatcher->start(10000); // poll every 10s - cheap, no need to go faster
}

void MainWindow::onGameDetected(const QString &displayName, const QString &applicationId)
{
    m_currentActivityName = displayName;
    m_playingLabel->setText("Playing " + displayName);

    QString status = m_statusCombo->currentData().toString();
    m_gateway->updatePresence(status, displayName, applicationId);
}

void MainWindow::onGameEnded()
{
    m_currentActivityName.clear();
    m_playingLabel->clear();

    QString status = m_statusCombo->currentData().toString();
    m_gateway->updatePresence(status);
}

// ---------------- Voice (Phase A - signaling only) ----------------

void MainWindow::onVoiceStateUpdate(const QJsonObject &data)
{
    // This dispatch fires for every member's voice state change in guilds
    // we're in. Track anyone in OUR voice channel specifically as a known
    // DAVE member, regardless of whether it's our own update - this is
    // what MLS welcome/proposal processing needs to recognize people who
    // joined but haven't spoken yet (Speaking events are too late/unreliable
    // for this).
    if (data.value("guild_id").toString() == m_pendingVoiceGuildId
        && data.value("channel_id").toString() == m_pendingVoiceChannelId) {
        QString memberUserId = data.value("user_id").toString();
        if (!memberUserId.isEmpty()) {
            m_voiceClient->addKnownMember(memberUserId);
        }
    }

    // The rest of this function only cares about OUR OWN voice state
    // (to get our session_id for the voice gateway handshake).
    if (data.value("user_id").toString() != m_currentUser.value("id").toString()) {
        return;
    }
    if (data.value("guild_id").toString() != m_pendingVoiceGuildId) {
        return;
    }

    m_pendingSessionId = data.value("session_id").toString();

    if (!m_pendingVoiceEndpoint.isEmpty() && !m_pendingVoiceToken.isEmpty()) {
        m_voiceClient->connectVoice(m_pendingVoiceEndpoint, m_pendingVoiceToken,
                                     m_pendingVoiceGuildId, m_pendingVoiceChannelId,
                                     m_currentUser.value("id").toString(),
                                     m_pendingSessionId);
    }
}

void MainWindow::onVoiceServerUpdate(const QJsonObject &data)
{
    if (data.value("guild_id").toString() != m_pendingVoiceGuildId) {
        return;
    }

    m_pendingVoiceEndpoint = data.value("endpoint").toString();
    m_pendingVoiceToken = data.value("token").toString();

    if (!m_pendingSessionId.isEmpty()) {
        m_voiceClient->connectVoice(m_pendingVoiceEndpoint, m_pendingVoiceToken,
                                     m_pendingVoiceGuildId, m_pendingVoiceChannelId,
                                     m_currentUser.value("id").toString(),
                                     m_pendingSessionId);
    }
}

void MainWindow::onVoiceHandshakeComplete()
{
    statusBar()->showMessage("Voice connected", 5000);
    m_voiceStatusLabel->setText("Connected to voice");
    m_muteButton->setChecked(false);
    m_deafenButton->setChecked(false);
    m_voiceBar->show();
}

void MainWindow::onVoiceError(const QString &reason)
{
    statusBar()->showMessage("Voice error: " + reason, 8000);
}

void MainWindow::onMuteClicked()
{
    bool muted = m_muteButton->isChecked();
    m_voiceClient->setMuted(muted);
    m_muteButton->setText(muted ? "Unmute" : "Mute");
}

void MainWindow::onDeafenClicked()
{
    bool deafened = m_deafenButton->isChecked();
    m_voiceClient->setDeafened(deafened);

    // Matches standard Discord UX: deafening also mutes your mic, since
    // there's little point talking if you can't hear anyone respond.
    if (deafened) {
        m_muteButton->setChecked(true);
        m_voiceClient->setMuted(true);
        m_muteButton->setText("Unmute");
    }
    m_deafenButton->setText(deafened ? "Undeafen" : "Deafen");
}

void MainWindow::onDisconnectVoiceClicked()
{
    if (!m_pendingVoiceGuildId.isEmpty()) {
        m_gateway->leaveVoiceChannel(m_pendingVoiceGuildId);
    }
    m_voiceClient->disconnectVoice();
    m_voiceBar->hide();
    m_muteButton->setChecked(false);
    m_muteButton->setText("Mute");
    m_deafenButton->setChecked(false);
    m_deafenButton->setText("Deafen");
    statusBar()->showMessage("Left voice channel", 3000);
}

void MainWindow::onVoiceSettingsClicked()
{
    auto *dialog = new VoiceSettingsDialog(m_voiceClient, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::onVoiceLog(const QString &line)
{
    // Route voice-specific log lines into the same debug log file the
    // gateway uses, so both handshakes can be read together chronologically.
    static QFile f("gateway-debug.log");
    if (!f.isOpen()) {
        f.open(QIODevice::Append | QIODevice::Text);
    }
    QTextStream out(&f);
    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "  [voice] " << line << "\n";
    out.flush();
}

// ---------------- Members, friends, DMs, profiles ----------------

void MainWindow::populateMemberList(const QString &guildId)
{
    m_memberList->clear();
    const QJsonArray members = m_guildMembers.value(guildId);
    for (const QJsonValue &v : members) {
        QJsonObject member = v.toObject();
        QJsonObject user = member.value("user").toObject();
        QString displayName = member.value("nick").toString();
        if (displayName.isEmpty()) {
            displayName = user.value("global_name").toString();
        }
        if (displayName.isEmpty()) {
            displayName = user.value("username").toString();
        }
        if (displayName.isEmpty()) {
            continue; // skip malformed entries rather than show a blank row
        }
        auto *item = new QListWidgetItem(displayName);
        item->setData(Qt::UserRole, user.value("id").toString());
        m_memberList->addItem(item);
    }
    if (members.isEmpty()) {
        m_memberList->addItem("(member list not available for this server yet)");
    }
}

void MainWindow::populateFriendsAndDMs()
{
    m_channelList->clear();
    m_messageView->clear();
    m_selectedChannelId.clear();
    m_messageInput->setEnabled(false);
    m_sendButton->setEnabled(false);

    if (!m_dmChannels.isEmpty()) {
        auto *dmHeader = new QListWidgetItem("— Direct Messages —");
        dmHeader->setFlags(Qt::NoItemFlags);
        m_channelList->addItem(dmHeader);

        for (const QJsonValue &v : m_dmChannels) {
            QJsonObject dm = v.toObject();
            QJsonArray recipients = dm.value("recipients").toArray();
            QString label = "(empty DM)";
            if (!recipients.isEmpty()) {
                label = recipients.first().toObject().value("username").toString();
            }
            auto *item = new QListWidgetItem(label);
            item->setData(Qt::UserRole, dm.value("id").toString());
            item->setData(Qt::UserRole + 1, "dm");
            m_channelList->addItem(item);
        }
    }

    if (!m_friendsList.isEmpty()) {
        auto *friendsHeader = new QListWidgetItem("— Friends —");
        friendsHeader->setFlags(Qt::NoItemFlags);
        m_channelList->addItem(friendsHeader);

        for (const QJsonValue &v : m_friendsList) {
            QJsonObject rel = v.toObject();
            if (rel.value("type").toInt() != 1) {
                continue; // 1 = accepted friend; skip pending/blocked for this simple list
            }
            QJsonObject user = rel.value("user").toObject();
            auto *item = new QListWidgetItem(user.value("username").toString());
            item->setData(Qt::UserRole, user.value("id").toString());
            item->setData(Qt::UserRole + 1, "friend");
            m_channelList->addItem(item);
        }
    }
}

void MainWindow::openDMChannel(const QJsonObject &dmChannel)
{
    m_selectedChannelId = dmChannel.value("id").toString();
    m_messageView->clear();
    m_messageView->addItem("Loading messages...");
    m_messageInput->setEnabled(true);
    m_sendButton->setEnabled(true);
    m_messageInput->setFocus();
    m_rest->fetchMessages(m_token, m_selectedChannelId);
}

void MainWindow::onMemberClicked(QListWidgetItem *item)
{
    QString userId = item->data(Qt::UserRole).toString();
    if (userId.isEmpty()) {
        return;
    }
    m_rest->fetchUserById(m_token, userId);
}

void MainWindow::onServerInfoClicked()
{
    QListWidgetItem *current = m_guildList->currentItem();
    if (!current) return;
    QString guildId = current->data(Qt::UserRole).toString();
    if (guildId == "@friends" || !m_fullGuildData.contains(guildId)) return;

    auto *dialog = new ServerProfileDialog(m_fullGuildData.value(guildId), this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::onRelationshipsFetched(const QJsonArray &friends)
{
    m_friendsList = friends;
    if (m_viewingFriends) {
        populateFriendsAndDMs();
    }
}

void MainWindow::onDMChannelsFetched(const QJsonArray &channels)
{
    m_dmChannels = channels;
    if (m_viewingFriends) {
        populateFriendsAndDMs();
    }
}

void MainWindow::onDMOpened(const QJsonObject &channel)
{
    // Add to our cached list if it's new, then open it.
    QString channelId = channel.value("id").toString();
    bool alreadyKnown = false;
    for (const QJsonValue &v : m_dmChannels) {
        if (v.toObject().value("id").toString() == channelId) {
            alreadyKnown = true;
            break;
        }
    }
    if (!alreadyKnown) {
        m_dmChannels.append(channel);
    }
    openDMChannel(channel);
}

void MainWindow::onUserProfileFetched(const QJsonObject &user)
{
    auto *dialog = new ProfileViewDialog(user, this);
    connect(dialog, &ProfileViewDialog::messageRequested, this, &MainWindow::onMessageProfileRequested);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::onUserProfileFetchFailed(const QString &reason)
{
    statusBar()->showMessage("Couldn't load profile: " + reason, 5000);
}

void MainWindow::onMessageProfileRequested(const QString &userId)
{
    m_rest->createOrOpenDM(m_token, userId);
}

void MainWindow::onMessageContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = m_messageView->itemAt(pos);
    if (!item) return;

    QString messageId = item->data(Qt::UserRole).toString();
    QString authorId = item->data(Qt::UserRole + 1).toString();
    QString rawContent = item->data(Qt::UserRole + 2).toString();
    if (messageId.isEmpty()) return; // placeholder rows ("Loading...", etc) have no id

    bool isOwnMessage = (authorId == m_currentUser.value("id").toString());

    QMenu menu(this);
    QAction *copyTextAction = menu.addAction("Copy Text");
    QAction *copyLinkAction = menu.addAction("Copy Message Link");
    QAction *editAction = isOwnMessage ? menu.addAction("Edit Message") : nullptr;
    QAction *deleteAction = isOwnMessage ? menu.addAction("Delete Message") : nullptr;

    QAction *chosen = menu.exec(m_messageView->mapToGlobal(pos));
    if (!chosen) return;

    if (chosen == copyTextAction) {
        QApplication::clipboard()->setText(rawContent);
        statusBar()->showMessage("Copied message text", 2000);
    } else if (chosen == copyLinkAction) {
        QListWidgetItem *guildItem = m_guildList->currentItem();
        QString guildId = guildItem ? guildItem->data(Qt::UserRole).toString() : "@me";
        QString link = QString("https://discord.com/channels/%1/%2/%3")
                            .arg(guildId, m_selectedChannelId, messageId);
        QApplication::clipboard()->setText(link);
        statusBar()->showMessage("Copied message link", 2000);
    } else if (chosen == editAction) {
        bool ok = false;
        QString newText = QInputDialog::getText(this, "Edit Message", "New content:",
                                                  QLineEdit::Normal, rawContent, &ok);
        if (ok && !newText.isEmpty() && newText != rawContent) {
            m_rest->editMessage(m_token, m_selectedChannelId, messageId, newText);
        }
    } else if (chosen == deleteAction) {
        auto reply = QMessageBox::question(this, "Delete Message",
                                            "Delete this message? This can't be undone.",
                                            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            m_rest->deleteMessage(m_token, m_selectedChannelId, messageId);
            delete item; // remove from view immediately, don't wait for a gateway echo
        }
    }
}
