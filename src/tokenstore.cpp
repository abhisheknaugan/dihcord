#include "tokenstore.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

TokenStore::TokenStore() {}

QString TokenStore::storagePath() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/session.bin";
}

bool TokenStore::saveToken(const QString &token)
{
#ifdef _WIN32
    QByteArray utf8 = token.toUtf8();

    DATA_BLOB in{};
    in.pbData = reinterpret_cast<BYTE *>(utf8.data());
    in.cbData = static_cast<DWORD>(utf8.size());

    DATA_BLOB out{};
    // CRYPTPROTECT_UI_FORBIDDEN: never show a Windows UI prompt for this.
    BOOL ok = CryptProtectData(&in, L"RipcordAlt", nullptr, nullptr, nullptr,
                                CRYPTPROTECT_UI_FORBIDDEN, &out);
    if (!ok) {
        return false;
    }

    QByteArray encrypted(reinterpret_cast<const char *>(out.pbData), out.cbData);
    SecureZeroMemory(utf8.data(), utf8.size());
    LocalFree(out.pbData);

    QFile f(storagePath());
    if (!f.open(QIODevice::WriteOnly)) {
        return false;
    }
    f.write(encrypted);
    f.close();
    return true;
#else
    Q_UNUSED(token);
    return false; // Windows-only for this phase, per project scope
#endif
}

QString TokenStore::loadToken() const
{
#ifdef _WIN32
    QFile f(storagePath());
    if (!f.open(QIODevice::ReadOnly)) {
        return QString();
    }
    QByteArray encrypted = f.readAll();
    f.close();

    DATA_BLOB in{};
    in.pbData = reinterpret_cast<BYTE *>(encrypted.data());
    in.cbData = static_cast<DWORD>(encrypted.size());

    DATA_BLOB out{};
    BOOL ok = CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr,
                                  CRYPTPROTECT_UI_FORBIDDEN, &out);
    if (!ok) {
        // Likely copied from another machine/user profile - treat as absent.
        return QString();
    }

    QByteArray decrypted(reinterpret_cast<const char *>(out.pbData), out.cbData);
    SecureZeroMemory(out.pbData, out.cbData);
    LocalFree(out.pbData);

    return QString::fromUtf8(decrypted);
#else
    return QString();
#endif
}

void TokenStore::clearToken()
{
    QFile::remove(storagePath());
}
