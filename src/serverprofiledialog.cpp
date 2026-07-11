#include "serverprofiledialog.h"

#include <QJsonArray>
#include <QLabel>
#include <QVBoxLayout>

ServerProfileDialog::ServerProfileDialog(const QJsonObject &guild, QWidget *parent)
    : QDialog(parent)
{
    QString name = guild.value("name").toString();
    setWindowTitle(name.isEmpty() ? "Server Info" : name);
    setMinimumWidth(320);

    auto *layout = new QVBoxLayout(this);

    auto *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(nameLabel);

    QString description = guild.value("description").toString();
    if (!description.isEmpty()) {
        auto *descLabel = new QLabel(description);
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("color: gray;");
        layout->addWidget(descLabel);
    }

    layout->addSpacing(8);

    int memberCount = guild.value("member_count").toInt(guild.value("members").toArray().size());
    int channelCount = guild.value("channels").toArray().size();

    auto *statsLabel = new QLabel(QString("%1 members  •  %2 channels").arg(memberCount).arg(channelCount));
    statsLabel->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(statsLabel);

    auto *idLabel = new QLabel("Server ID: " + guild.value("id").toString());
    idLabel->setStyleSheet("color: gray; font-size: 10px;");
    layout->addWidget(idLabel);
}
