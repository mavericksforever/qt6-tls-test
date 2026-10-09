// Minimal Qt 6 HTTPS request.
//
// Makes one GET request on launch and prints what Qt's TLS layer reports. Under
// AquaTransport on OS X 10.9 the request fails with
// "SSL handshake failed: Protocol version mismatch", because Qt's Secure Transport
// backend checks the negotiated version itself and the hooked
// SSLGetNegotiatedProtocolVersion reports TLS 1.0.
//
// The checkbox switches the request to QSsl::AnyProtocol, which skips that version
// check. It is only a diagnostic: with it on, the request goes through and Qt shows
// the protocol it was told was negotiated.

#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QSysInfo>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdio>

static QString protocolName(QSsl::SslProtocol p)
{
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_DEPRECATED
    switch (p) {
    case QSsl::TlsV1_0: return QStringLiteral("TLS 1.0");
    case QSsl::TlsV1_1: return QStringLiteral("TLS 1.1");
    case QSsl::TlsV1_2: return QStringLiteral("TLS 1.2");
    case QSsl::TlsV1_3: return QStringLiteral("TLS 1.3");
    case QSsl::AnyProtocol: return QStringLiteral("AnyProtocol");
    case QSsl::SecureProtocols: return QStringLiteral("SecureProtocols (TLS 1.2+, Qt default)");
    case QSsl::UnknownProtocol: return QStringLiteral("unknown");
    default: return QStringLiteral("enum value %1").arg(int(p));
    }
    QT_WARNING_POP
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle(QStringLiteral("Qt 6 HTTPS Test"));
    window.resize(560, 360);

    const QString defaultUrl = argc > 1 ? QString::fromLocal8Bit(argv[1])
                                        : QStringLiteral("https://login.microsoftonline.com/");
    auto *urlEdit = new QLineEdit(defaultUrl);
    auto *anyProtocol = new QCheckBox(QStringLiteral("Accept any TLS version (diagnostic)"));
    auto *sendButton = new QPushButton(QStringLiteral("Send Request"));
    auto *log = new QPlainTextEdit;
    log->setReadOnly(true);

    auto *layout = new QVBoxLayout(&window);
    layout->addWidget(urlEdit);
    layout->addWidget(anyProtocol);
    layout->addWidget(sendButton);
    layout->addWidget(log);

    auto print = [log](const QString &line) {
        log->appendPlainText(line);
        std::fprintf(stderr, "%s\n", qPrintable(line));
    };

    print(QStringLiteral("Qt %1 on %2").arg(QString::fromLatin1(qVersion()), QSysInfo::prettyProductName()));
    print(QStringLiteral("TLS backend: %1").arg(QSslSocket::activeBackend()));

    auto *manager = new QNetworkAccessManager(&window);

    QObject::connect(sendButton, &QPushButton::clicked, &window, [=] {
        QNetworkRequest request(QUrl(urlEdit->text().trimmed()));
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        if (anyProtocol->isChecked())
            config.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(config);

        print(QString());
        print(QStringLiteral("GET %1").arg(request.url().toString()));
        print(QStringLiteral("Requested protocol: %1").arg(protocolName(config.protocol())));

        QNetworkReply *reply = manager->get(request);
        QObject::connect(reply, &QNetworkReply::finished, reply, [=] {
            const QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            if (status.isValid()) {
                // Any HTTP status, including 4xx, means the TLS connection itself worked.
                const QSslConfiguration session = reply->sslConfiguration();
                print(QStringLiteral("OK: HTTP %1").arg(status.toInt()));
                print(QStringLiteral("Negotiated protocol as reported to Qt: %1")
                          .arg(protocolName(session.sessionProtocol())));
                print(QStringLiteral("Negotiated cipher as reported to Qt: %1")
                          .arg(session.sessionCipher().name()));
            } else {
                print(QStringLiteral("FAILED: %1").arg(reply->errorString()));
            }
            reply->deleteLater();
        });
    });

    window.show();
    QTimer::singleShot(0, sendButton, &QPushButton::click);
    return app.exec();
}
