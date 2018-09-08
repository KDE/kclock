#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <KLocalizedString>
#include <KLocalizedContext>
#include <QCommandLineParser>
#include <QMetaObject>

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
    qmlRegisterType<TimeZoneSelectorModel>("app.test", 1, 0, "TimeZoneSelectorModel");
    qmlRegisterType<TimeZoneFilterModel>("app.test", 1, 0, "TimeZoneModel");
    QGuiApplication app(argc, argv);
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
