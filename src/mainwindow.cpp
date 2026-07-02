#include "mainwindow.h"
#include "profiledialog.h"

#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
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

    profileBar->addWidget(m_usernameLabel);
    profileBar->addWidget(m_playingLabel);
    profileBar->addStretch();
    profileBar->addWidget(m_statusCombo);
    profileBar->addWidget(m_editProfileButton);
    rootLayout->addLayout(profileBar);

    connect(m_statusCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onStatusChanged);
    connect(m_editProfileButton, &QPushButton::clicked, this, &MainWindow::onEditProfileClicked);

    // --- Main three-pane splitter ---
    auto *splitter = new QSplitter();
    m_guildList = new QListWidget();
    m_channelList = new QListWidget();

    auto *messagePane = new QWidget();
    auto *messagePaneLayout = new QVBoxLayout(messagePane);
    messagePaneLayout->setContentsMargins(0, 0, 0, 0);
    m_messageView = new QListWidget();
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

    rootLayout->addWidget(splitter);
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
}

// ---------------- Guilds / channels ----------------

void MainWindow::onGuildsFetched(const QByteArray &jsonPayload)
{
    QJsonArray guilds = QJsonDocument::fromJson(jsonPayload).array();
    m_guildList->clear();
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

    QListWidgetItem *current = m_guildList->currentItem();
    if (current && current->data(Qt::UserRole).toString() == guildId) {
        populateChannelsForGuild(guildId);
    }
}

void MainWindow::onGatewayError(const QString &reason)
{
    statusBar()->showMessage("Gateway: " + reason, 5000);
}

void MainWindow::onGuildSelected(QListWidgetItem *item)
{
    QString guildId = item->data(Qt::UserRole).toString();
    // Ask Discord to actually push this guild's data - see
    // GatewayClient::requestGuildSubscription for why this is needed.
    m_gateway->requestGuildSubscription(guildId);
    populateChannelsForGuild(guildId);
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
    if (channelType == "voice") {
        QListWidgetItem *guildItem = m_guildList->currentItem();
        if (!guildItem) {
            return;
        }
        QString guildId = guildItem->data(Qt::UserRole).toString();

        m_pendingVoiceGuildId = guildId;
        m_pendingSessionId.clear();
        m_pendingVoiceEndpoint.clear();
        m_pendingVoiceToken.clear();

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
    QString author = message.value("author").toObject().value("username").toString();
    QString content = message.value("content").toString();
    if (content.isEmpty()) {
        content = "(no text content - attachment/embed only, not rendered in this phase)";
    }
    m_messageView->addItem(QString("%1: %2").arg(author, content));
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
    // we're in, not just our own - filter to only our own user's update.
    if (data.value("user_id").toString() != m_currentUser.value("id").toString()) {
        return;
    }
    if (data.value("guild_id").toString() != m_pendingVoiceGuildId) {
        return;
    }

    m_pendingSessionId = data.value("session_id").toString();

    if (!m_pendingVoiceEndpoint.isEmpty() && !m_pendingVoiceToken.isEmpty()) {
        m_voiceClient->connectVoice(m_pendingVoiceEndpoint, m_pendingVoiceToken,
                                     m_pendingVoiceGuildId, m_currentUser.value("id").toString(),
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
                                     m_pendingVoiceGuildId, m_currentUser.value("id").toString(),
                                     m_pendingSessionId);
    }
}

void MainWindow::onVoiceHandshakeComplete()
{
    statusBar()->showMessage("Voice connected - handshake complete (audio not implemented yet)", 8000);
}

void MainWindow::onVoiceError(const QString &reason)
{
    statusBar()->showMessage("Voice error: " + reason, 8000);
}

void MainWindow::onVoiceLog(const QString &line)
{
    // Route voice-specific log lines into the same debug log file the
    // gateway uses, so both handshakes can be read together chronologically.
    QFile f("gateway-debug.log");
    f.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);
    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "  [voice] " << line << "\n";
}
