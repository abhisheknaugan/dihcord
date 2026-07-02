#include "profiledialog.h"

#include <QBuffer>
#include <QFileDialog>
#include <QFormLayout>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

ProfileDialog::ProfileDialog(const QJsonObject &currentUser, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Edit profile");
    setMinimumWidth(360);

    m_originalUsername = currentUser.value("username").toString();
    m_originalBio = currentUser.value("bio").toString();

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    m_usernameEdit = new QLineEdit(m_originalUsername);
    form->addRow("Username", m_usernameEdit);

    m_bioEdit = new QTextEdit(m_originalBio);
    m_bioEdit->setMaximumHeight(80);
    form->addRow("About me", m_bioEdit);

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Only needed if changing username");
    form->addRow("Current password", m_passwordEdit);

    layout->addLayout(form);

    auto *avatarRow = new QVBoxLayout();
    m_avatarPreviewLabel = new QLabel("(no new avatar selected)");
    m_chooseAvatarButton = new QPushButton("Choose new avatar...");
    avatarRow->addWidget(m_avatarPreviewLabel);
    avatarRow->addWidget(m_chooseAvatarButton);
    layout->addLayout(avatarRow);
    connect(m_chooseAvatarButton, &QPushButton::clicked, this, &ProfileDialog::onChooseAvatarClicked);

    auto *saveButton = new QPushButton("Save changes");
    layout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &ProfileDialog::onSaveClicked);
}

void ProfileDialog::onChooseAvatarClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose avatar", QString(),
                                                  "Images (*.png *.jpg *.jpeg)");
    if (path.isEmpty()) {
        return;
    }

    QImage image(path);
    if (image.isNull()) {
        QMessageBox::warning(this, "Couldn't load image", "That file couldn't be read as an image.");
        return;
    }

    // Discord avatars are displayed small; downscaling here keeps the
    // PATCH payload tiny and avoids uploading a multi-MB source photo.
    QImage scaled = image.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "PNG");

    m_pendingAvatarDataUri = "data:image/png;base64," + QString::fromLatin1(bytes.toBase64());
    m_avatarPreviewLabel->setText(QString("New avatar selected (%1 KB)").arg(bytes.size() / 1024));
}

void ProfileDialog::onSaveClicked()
{
    QJsonObject patch;

    QString newUsername = m_usernameEdit->text().trimmed();
    if (newUsername != m_originalUsername && !newUsername.isEmpty()) {
        patch["username"] = newUsername;
        if (m_passwordEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Password required",
                                  "Discord requires your current password to change your username.");
            return;
        }
        patch["password"] = m_passwordEdit->text();
    }

    QString newBio = m_bioEdit->toPlainText();
    if (newBio != m_originalBio) {
        patch["bio"] = newBio;
    }

    if (!m_pendingAvatarDataUri.isEmpty()) {
        patch["avatar"] = m_pendingAvatarDataUri;
    }

    if (patch.isEmpty()) {
        accept(); // nothing changed, just close
        return;
    }

    emit saveRequested(patch);
    accept();
}
