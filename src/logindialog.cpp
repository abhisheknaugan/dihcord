#include "logindialog.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , m_rest(new RestClient(this))
{
    setWindowTitle("Log in");
    setMinimumWidth(340);

    m_stack = new QStackedWidget(this);
    m_rememberCheck = new QCheckBox("Remember me on this device", this);
    m_rememberCheck->setChecked(true);

    buildCredentialsPage();
    buildMfaPage();
    buildCaptchaFallbackPage();

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_stack);
    layout->addWidget(m_rememberCheck);

    connect(m_rest, &RestClient::loginSucceeded, this, &LoginDialog::onLoginSucceeded);
    connect(m_rest, &RestClient::mfaRequired, this, &LoginDialog::onMfaRequired);
    connect(m_rest, &RestClient::captchaRequired, this, &LoginDialog::onCaptchaRequired);
    connect(m_rest, &RestClient::loginFailed, this, &LoginDialog::onLoginFailed);
}

void LoginDialog::buildCredentialsPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    layout->addWidget(new QLabel("Email"));
    m_emailEdit = new QLineEdit;
    layout->addWidget(m_emailEdit);

    layout->addWidget(new QLabel("Password"));
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_passwordEdit);

    m_credentialsError = new QLabel;
    m_credentialsError->setStyleSheet("color: #d9534f;");
    m_credentialsError->setWordWrap(true);
    m_credentialsError->hide();
    layout->addWidget(m_credentialsError);

    auto *submit = new QPushButton("Log in");
    layout->addWidget(submit);
    connect(submit, &QPushButton::clicked, this, &LoginDialog::onSubmitClicked);

    auto *tokenLink = new QPushButton("Use a token instead");
    tokenLink->setFlat(true);
    layout->addWidget(tokenLink);
    connect(tokenLink, &QPushButton::clicked, this, [this]() { m_stack->setCurrentIndex(2); });

    m_stack->addWidget(page); // index 0
}

void LoginDialog::buildMfaPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    layout->addWidget(new QLabel("Enter the 6-digit code from your authenticator app"));
    m_mfaCodeEdit = new QLineEdit;
    m_mfaCodeEdit->setMaxLength(6);
    layout->addWidget(m_mfaCodeEdit);

    m_mfaError = new QLabel;
    m_mfaError->setStyleSheet("color: #d9534f;");
    m_mfaError->setWordWrap(true);
    m_mfaError->hide();
    layout->addWidget(m_mfaError);

    auto *submit = new QPushButton("Verify");
    layout->addWidget(submit);
    connect(submit, &QPushButton::clicked, this, &LoginDialog::onMfaSubmitClicked);

    m_stack->addWidget(page); // index 1
}

void LoginDialog::buildCaptchaFallbackPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    m_tokenHint = new QLabel(
        "Discord is asking for a captcha here, which this client won't try to solve "
        "(it would need an embedded browser, which defeats the point of staying "
        "lightweight). Instead: log into Discord in your normal web browser, open "
        "developer tools, and copy your auth token from a request header. Paste it below."
    );
    m_tokenHint->setWordWrap(true);
    layout->addWidget(m_tokenHint);

    m_tokenEdit = new QLineEdit;
    m_tokenEdit->setPlaceholderText("Paste token here");
    layout->addWidget(m_tokenEdit);

    auto *submit = new QPushButton("Continue");
    layout->addWidget(submit);
    connect(submit, &QPushButton::clicked, this, &LoginDialog::onTokenSubmitClicked);

    m_stack->addWidget(page); // index 2
}

void LoginDialog::onSubmitClicked()
{
    m_credentialsError->hide();
    m_rest->login(m_emailEdit->text().trimmed(), m_passwordEdit->text());
}

void LoginDialog::onMfaSubmitClicked()
{
    m_mfaError->hide();
    m_rest->submitMfaCode(m_mfaTicket, m_mfaCodeEdit->text().trimmed());
}

void LoginDialog::onTokenSubmitClicked()
{
    QString token = m_tokenEdit->text().trimmed();
    if (!token.isEmpty()) {
        emit loginSucceeded(token, m_rememberCheck->isChecked());
        accept();
    }
}

void LoginDialog::onLoginSucceeded(const QString &token)
{
    emit loginSucceeded(token, m_rememberCheck->isChecked());
    accept();
}

void LoginDialog::onMfaRequired(const QString &ticket)
{
    m_mfaTicket = ticket;
    m_stack->setCurrentIndex(1);
}

void LoginDialog::onCaptchaRequired()
{
    m_stack->setCurrentIndex(2);
}

void LoginDialog::onLoginFailed(const QString &reason)
{
    m_credentialsError->setText(reason);
    m_credentialsError->show();
}
