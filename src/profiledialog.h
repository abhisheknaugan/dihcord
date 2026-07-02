#pragma once

#include <QDialog>
#include <QJsonObject>

class QLineEdit;
class QTextEdit;
class QLabel;
class QPushButton;

// Simple profile editor: username, bio, avatar (file picker -> base64
// data URI, as Discord's API expects), and current password (only
// required by Discord if username/email is being changed).
//
// Status (online/idle/dnd/invisible) is handled separately in
// MainWindow's status dropdown - that's a live gateway presence update,
// not a REST profile field, so it doesn't belong in this dialog.
class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
    // `currentUser` is the JSON object from GET /users/@me, used to
    // pre-fill the fields.
    explicit ProfileDialog(const QJsonObject &currentUser, QWidget *parent = nullptr);

signals:
    // Emits only the fields that actually changed, ready to hand to
    // RestClient::updateProfile().
    void saveRequested(const QJsonObject &patchFields);

private slots:
    void onChooseAvatarClicked();
    void onSaveClicked();

private:
    QLineEdit *m_usernameEdit;
    QTextEdit *m_bioEdit;
    QLineEdit *m_passwordEdit;
    QLabel *m_avatarPreviewLabel;
    QPushButton *m_chooseAvatarButton;

    QString m_originalUsername;
    QString m_originalBio;
    QString m_pendingAvatarDataUri; // empty until user picks a new image
};
