#pragma once

#include <QJsonArray>
#include <QString>

// Local disk cache for guild data (channels, members, voice states,
// basic metadata). The whole point: an account in many large servers
// would otherwise need to keep ALL of that in RAM permanently, even for
// servers never actually viewed. This writes it to a small SQLite file
// instead, and callers only pull back what they need, when they need it
// (i.e. only the currently-open server's data lives in memory at once).
class DataCache
{
public:
    DataCache();

    bool open(); // call once at startup

    void storeGuildChannels(const QString &guildId, const QJsonArray &channels);
    QJsonArray fetchGuildChannels(const QString &guildId);

    void storeGuildMembers(const QString &guildId, const QJsonArray &members);
    QJsonArray fetchGuildMembers(const QString &guildId);

    void storeGuildVoiceStates(const QString &guildId, const QJsonArray &voiceStates);
    QJsonArray fetchGuildVoiceStates(const QString &guildId);

    struct GuildMeta {
        QString name;
        QString description;
        int memberCount = 0;
        int channelCount = 0;
        bool valid = false;
    };
    void storeGuildMeta(const QString &guildId, const QString &name, const QString &description,
                        int memberCount, int channelCount);
    GuildMeta fetchGuildMeta(const QString &guildId);

private:
    bool m_opened = false;
};
