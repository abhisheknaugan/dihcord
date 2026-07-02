#pragma once

#include <QDialog>

#include "restclient.h"

class QLineEdit;
class QLabel;
class QPushButton;
class QStackedWidget;
class QCheckBox;

// Login flow:
//   1. Email + password form (default view).
//   2. On success -> emit loginSucceeded, done.
//   3. On MFA required -> swap to a "enter your 6-digit code" view.
//   4. On captcha required -> swap to a "paste your token instead" view,
//      since we deliberately don't embed a browser/GPU compositor to
//      solve captchas. See restclient.h for why.
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

signals:
    void loginSucceeded(const QString &token, bool remember);

private slots:
    void onSubmitClicked();
    void onMfaSubmitClicked();
    void onTokenSubmitClicked();

    void onLoginSucceeded(const QString &token);
    void onMfaRequired(const QString &ticket);
    void onCaptchaRequired();
    void onLoginFailed(const QString &reason);

private:
    void buildCredentialsPage();
    void buildMfaPage();
    void buildCaptchaFallbackPage();

    RestClient *m_rest;
    QStackedWidget *m_stack;
    QCheckBox *m_rememberCheck;

    // Credentials page
    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
    QLabel *m_credentialsError;

    // MFA page
    QLineEdit *m_mfaCodeEdit;
    QLabel *m_mfaError;
    QString m_mfaTicket;

    // Captcha fallback page
    QLineEdit *m_tokenEdit;
    QLabel *m_tokenHint;
};
