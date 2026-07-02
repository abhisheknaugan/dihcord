#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QTimer>

// Polls the running-processes list on a timer (no hooks, no GPU, minimal
// CPU) and checks executable names against Discord's public detectable
// game list. This is the same basic technique Discord's own client uses
// for "Playing X" rich presence, minus the OS-level window-focus tricks.
class ProcessWatcher : public QObject
{
    Q_OBJECT
public:
    explicit ProcessWatcher(QObject *parent = nullptr);

    // key: lowercased executable name (e.g. "valorant-win64-shipping.exe")
    // value: {applicationId, displayName}
    struct GameInfo {
        QString applicationId;
        QString displayName;
    };

    void setDetectableGames(const QMap<QString, GameInfo> &games);
    void start(int pollIntervalMs = 10000);
    void stop();

signals:
    void gameDetected(const QString &displayName, const QString &applicationId);
    void gameEnded();

private slots:
    void poll();

private:
    QStringList runningExecutableNames() const; // Windows: via CreateToolhelp32Snapshot

    QTimer *m_timer;
    QMap<QString, GameInfo> m_detectable;
    QString m_currentlyDetected; // executable name of whatever we last reported, empty if none
};
