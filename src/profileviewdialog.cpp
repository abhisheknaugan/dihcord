#include "profileviewdialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ProfileViewDialog::ProfileViewDialog(const QJsonObject &user, QWidget *parent)
    : QDialog(parent)
{
    m_userId = user.value("id").toString();

    QString username = user.value("username").toString();
    QString globalName = user.value("global_name").toString();
    QString bio = user.value("bio").toString();

    setWindowTitle(username.isEmpty() ? "Profile" : username);
    setMinimumWidth(320);

    auto *layout = new QVBoxLayout(this);

    auto *nameLabel = new QLabel(globalName.isEmpty() ? username : QString("%1  (@%2)").arg(globalName, username));
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(nameLabel);

    if (!bio.isEmpty()) {
        auto *bioLabel = new QLabel(bio);
        bioLabel->setWordWrap(true);
        bioLabel->setStyleSheet("color: gray;");
        layout->addWidget(bioLabel);
    }

    layout->addSpacing(8);

    auto *idLabel = new QLabel("User ID: " + m_userId);
    idLabel->setStyleSheet("color: gray; font-size: 10px;");
    layout->addWidget(idLabel);

    layout->addSpacing(8);

    auto *messageButton = new QPushButton("Message");
    layout->addWidget(messageButton);
    connect(messageButton, &QPushButton::clicked, this, [this]() {
        emit messageRequested(m_userId);
        accept();
    });
}
