#pragma once

#include <QDialog>
#include <QJsonObject>

// Simple server info popup - name, description, member count, when you
// joined. Uses whatever's already cached from GUILD_CREATE, no extra
// network round-trip needed for the basics.
class ServerProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ServerProfileDialog(const QJsonObject &guild, QWidget *parent = nullptr);
};
