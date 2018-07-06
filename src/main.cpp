#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <KLocalizedString>
#include <KLocalizedContext>
#include <QCommandLineParser>
#include <QMetaObject>
#include <QTimeZone>
#include<QDebug>

QCommandLineParser* createParser()
{
    QCommandLineParser* parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addHelpOption();
    return parser;
}

int main(int argc, char *argv[])
{
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
    
    QStringList list;
    QList<QByteArray> ids = QTimeZone::availableTimeZoneIds();
    foreach(QByteArray id, ids) {
        list.append(QTimeZone(id).displayName(QDateTime()));
    }
    engine.rootContext()->setContextProperty("timezoneModel", QVariant::fromValue(list));
    qDebug() << list;
    
    return app.exec();
}
