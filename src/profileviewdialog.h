#pragma once

#include <QDialog>
#include <QJsonObject>

class QLabel;
class QPushButton;

// Read-only profile view for someone else (member list click, friend
// click, etc). Shows whatever we have - username, bio/about-me, status -
// and offers a "Message" button that asks MainWindow to open/create a DM.
class ProfileViewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfileViewDialog(const QJsonObject &user, QWidget *parent = nullptr);

signals:
    void messageRequested(const QString &userId);

private:
    QString m_userId;
};
