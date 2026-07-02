#pragma once

#include <QString>

// Stores the Discord auth token encrypted at rest using Windows DPAPI
// (CryptProtectData / CryptUnprotectData). DPAPI ties the encryption to
// the current Windows user account, so the encrypted blob is useless if
// copied to another machine or user profile. We never write the raw
// token to disk in plaintext.
class TokenStore
{
public:
    TokenStore();

    // Encrypts and writes the token to the local app data path.
    // Returns false if encryption or the file write failed.
    bool saveToken(const QString &token);

    // Reads and decrypts the stored token, if any.
    // Returns an empty string if no token is stored or decryption failed
    // (e.g. file was copied from another machine/user).
    QString loadToken() const;

    // Removes the stored token (used on logout).
    void clearToken();

private:
    QString storagePath() const;
};
