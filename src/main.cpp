#include <QApplication>
#include <QSurfaceFormat>
#include <cstdlib>

#include "logindialog.h"
#include "mainwindow.h"
#include "tokenstore.h"

int main(int argc, char *argv[])
{
    // --- Force CPU-only / no-GPU rendering, before QApplication exists ---
    // Belt-and-suspenders: even though we only use Qt Widgets (raster by
    // default), some Qt builds probe ANGLE/OpenGL at startup for the
    // platform integration. These env vars keep that from happening.
    qputenv("QT_QUICK_BACKEND", "software");
    qputenv("QT_OPENGL", "software");
    qputenv("QT_ANGLE_PLATFORM", "d3d11warp"); // if ANGLE is ever probed, use WARP (CPU) not a real GPU device

    QApplication app(argc, argv);
    app.setApplicationName("RipcordAlt");
    app.setOrganizationName("RipcordAlt");

    // --- Phase 1 flow: try a stored token first, else show login ---
    TokenStore tokenStore;
    QString existingToken = tokenStore.loadToken();

    MainWindow *mainWindow = nullptr;

    auto launchMainWindow = [&](const QString &token) {
        mainWindow = new MainWindow(token);
        mainWindow->show();
    };

    if (!existingToken.isEmpty()) {
        launchMainWindow(existingToken);
    } else {
        LoginDialog login;
        QObject::connect(&login, &LoginDialog::loginSucceeded, [&](const QString &token, bool remember) {
            if (remember) {
                tokenStore.saveToken(token);
            }
            launchMainWindow(token);
        });

        if (login.exec() != QDialog::Accepted && !mainWindow) {
            return 0; // user closed the login dialog without success
        }
    }

    return app.exec();
}
