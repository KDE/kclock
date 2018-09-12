#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QMetaObject>

#include <KLocalizedString>
#include <KLocalizedContext>
#include <KAboutData>
#include <KConfig>

#include "timezoneselectormodel.h"

QCommandLineParser* createParser()
{
    QCommandLineParser* parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addHelpOption();
    return parser;
}

int main(int argc, char *argv[])
{
    KLocalizedString::setApplicationDomain("kirigamiclock");
    KAboutData aboutData("kirigamiclock", "Clock", "0.1", "Clock for Plasma Mobile", KAboutLicense::GPL);
    KAboutData::setApplicationData(aboutData);

    QApplication app(argc, argv);
    qmlRegisterType<TimeZoneSelectorModel>("app.test", 1, 0, "TimeZoneSelectorModel");
    qmlRegisterType<TimeZoneFilterModel>("app.test", 1, 0, "TimeZoneModel");
    QQmlApplicationEngine engine(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);
        if(parser->isSet(QStringLiteral("page"))) {
            QObject* rootObject = engine.rootObjects().first();
            QMetaObject::invokeMethod(rootObject, "switchToPage", Q_ARG(QVariant, parser->value("page")));
        }
    }
    return app.exec();
}
