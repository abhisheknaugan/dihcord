#include "restclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

RestClient::RestClient(QObject *parent)
    : QObject(parent)
    , m_net(new QNetworkAccessManager(this))
{
}

void RestClient::login(const QString &email, const QString &password)
{
    QUrl url(QString("%1/auth/login").arg(kApiBase));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // NOTE: a real client also needs an X-Super-Properties header describing
    // a client fingerprint. Omitted in this skeleton; we'll add it once
    // we're validating real login responses end-to-end.

    QJsonObject body{
        {"login", email},
        {"password", password},
        {"undelete", false}
    };

    QNetworkReply *reply = m_net->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleLoginReply(reply);
        reply->deleteLater();
    });
}

void RestClient::submitMfaCode(const QString &ticket, const QString &code)
{
    QUrl url(QString("%1/auth/mfa/totp").arg(kApiBase));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body{
        {"code", code},
        {"ticket", ticket}
    };

    QNetworkReply *reply = m_net->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleLoginReply(reply);
        reply->deleteLater();
    });
}

void RestClient::handleLoginReply(QNetworkReply *reply)
{
    QByteArray raw = reply->readAll();
    QJsonObject obj = QJsonDocument::fromJson(raw).object();

    if (reply->error() != QNetworkReply::NoError) {
        // Discord returns captcha challenges as a 400 with a captcha_key field,
        // not as a distinct HTTP status - so we check the body regardless of
        // the transport-level error code.
        if (obj.contains("captcha_key")) {
            emit captchaRequired();
            return;
        }
        emit loginFailed(reply->errorString());
        return;
    }

    if (obj.contains("token") && !obj.value("token").isNull()) {
        emit loginSucceeded(obj.value("token").toString());
        return;
    }

    if (obj.value("mfa").toBool()) {
        emit mfaRequired(obj.value("ticket").toString());
        return;
    }

    if (obj.contains("captcha_key")) {
        emit captchaRequired();
        return;
    }

    emit loginFailed("Unrecognized login response - Discord may have changed this endpoint.");
}

void RestClient::fetchGuilds(const QString &token)
{
    QUrl url(QString("%1/users/@me/guilds").arg(kApiBase));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit guildsFetchFailed(reply->errorString());
        } else {
            emit guildsFetched(reply->readAll());
        }
        reply->deleteLater();
    });
}

void RestClient::fetchMessages(const QString &token, const QString &channelId, int limit)
{
    QUrl url(QString("%1/channels/%2/messages?limit=%3").arg(kApiBase, channelId).arg(limit));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, channelId]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit messagesFetchFailed(channelId, reply->errorString());
        } else {
            QJsonArray messages = QJsonDocument::fromJson(reply->readAll()).array();
            emit messagesFetched(channelId, messages);
        }
        reply->deleteLater();
    });
}

void RestClient::sendMessage(const QString &token, const QString &channelId, const QString &content)
{
    QUrl url(QString("%1/channels/%2/messages").arg(kApiBase, channelId));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", token.toUtf8());

    QJsonObject body{{"content", content}};

    QNetworkReply *reply = m_net->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply, channelId]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit messageSendFailed(channelId, reply->errorString());
        }
        // On success we don't emit anything here - the message shows up
        // via the gateway's own MESSAGE_CREATE echo, same as any other
        // message, which keeps a single code path for rendering messages.
        reply->deleteLater();
    });
}

void RestClient::fetchCurrentUser(const QString &token)
{
    QUrl url(QString("%1/users/@me").arg(kApiBase));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit currentUserFetchFailed(reply->errorString());
        } else {
            emit currentUserFetched(QJsonDocument::fromJson(reply->readAll()).object());
        }
        reply->deleteLater();
    });
}

void RestClient::updateProfile(const QString &token, const QJsonObject &patchFields)
{
    QUrl url(QString("%1/users/@me").arg(kApiBase));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->sendCustomRequest(req, "PATCH", QJsonDocument(patchFields).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit profileUpdateFailed(reply->errorString());
        } else {
            emit profileUpdated(QJsonDocument::fromJson(reply->readAll()).object());
        }
        reply->deleteLater();
    });
}

void RestClient::fetchDetectableGames()
{
    // Public endpoint - same data Discord's own client uses to know which
    // running .exe corresponds to which "Playing X" rich presence entry.
    // No Authorization header needed.
    QUrl url(QString("%1/applications/detectable").arg(kApiBase));
    QNetworkRequest req(url);

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit detectableGamesFetchFailed(reply->errorString());
        } else {
            emit detectableGamesFetched(QJsonDocument::fromJson(reply->readAll()).array());
        }
        reply->deleteLater();
    });
}

void RestClient::fetchRelationships(const QString &token)
{
    QUrl url(QString("%1/users/@me/relationships").arg(kApiBase));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        emit relationshipsFetched(QJsonDocument::fromJson(reply->readAll()).array());
        reply->deleteLater();
    });
}

void RestClient::fetchDMChannels(const QString &token)
{
    QUrl url(QString("%1/users/@me/channels").arg(kApiBase));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        emit dmChannelsFetched(QJsonDocument::fromJson(reply->readAll()).array());
        reply->deleteLater();
    });
}

void RestClient::createOrOpenDM(const QString &token, const QString &recipientUserId)
{
    QUrl url(QString("%1/users/@me/channels").arg(kApiBase));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", token.toUtf8());

    QJsonObject body{{"recipient_id", recipientUserId}};

    QNetworkReply *reply = m_net->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        emit dmOpened(QJsonDocument::fromJson(reply->readAll()).object());
        reply->deleteLater();
    });
}

void RestClient::fetchUserById(const QString &token, const QString &userId)
{
    QUrl url(QString("%1/users/%2").arg(kApiBase, userId));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit userProfileFetchFailed(reply->errorString());
        } else {
            emit userProfileFetched(QJsonDocument::fromJson(reply->readAll()).object());
        }
        reply->deleteLater();
    });
}

void RestClient::editMessage(const QString &token, const QString &channelId, const QString &messageId,
                              const QString &newContent)
{
    QUrl url(QString("%1/channels/%2/messages/%3").arg(kApiBase, channelId, messageId));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", token.toUtf8());

    QJsonObject body{{"content", newContent}};
    QNetworkReply *reply = m_net->sendCustomRequest(req, "PATCH", QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        reply->deleteLater(); // the edited message shows up via the gateway's own MESSAGE_UPDATE, no need to handle here
    });
}

void RestClient::deleteMessage(const QString &token, const QString &channelId, const QString &messageId)
{
    QUrl url(QString("%1/channels/%2/messages/%3").arg(kApiBase, channelId, messageId));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = m_net->deleteResource(req);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        reply->deleteLater();
    });
}
