#include "processwatcher.h"

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

ProcessWatcher::ProcessWatcher(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &ProcessWatcher::poll);
}

void ProcessWatcher::setDetectableGames(const QMap<QString, GameInfo> &games)
{
    m_detectable = games;
}

void ProcessWatcher::start(int pollIntervalMs)
{
    m_timer->start(pollIntervalMs);
    poll(); // check immediately instead of waiting for the first interval
}

void ProcessWatcher::stop()
{
    m_timer->stop();
}

QStringList ProcessWatcher::runningExecutableNames() const
{
    QStringList names;
#ifdef _WIN32
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return names;
    }

    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &entry)) {
        do {
            names.append(QString::fromWCharArray(entry.szExeFile).toLower());
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
#endif
    return names;
}

void ProcessWatcher::poll()
{
    if (m_detectable.isEmpty()) {
        return; // detectable-games list hasn't loaded yet
    }

    const QStringList running = runningExecutableNames();

    // If whatever we last reported is still running, nothing to do.
    if (!m_currentlyDetected.isEmpty() && running.contains(m_currentlyDetected)) {
        return;
    }

    // Look for any currently-running exe that matches the detectable list.
    for (const QString &exe : running) {
        if (m_detectable.contains(exe)) {
            const GameInfo &info = m_detectable.value(exe);
            m_currentlyDetected = exe;
            emit gameDetected(info.displayName, info.applicationId);
            return;
        }
    }

    // Nothing matched - if we previously had something detected, it just closed.
    if (!m_currentlyDetected.isEmpty()) {
        m_currentlyDetected.clear();
        emit gameEnded();
    }
}
