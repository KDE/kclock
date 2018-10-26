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
#include "alarmsmodel.h"

QCommandLineParser* createParser()
{
    QCommandLineParser* parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addHelpOption();
    return parser;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    KLocalizedString::setApplicationDomain("kirigamiclock");
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KAboutData aboutData("kirigamiclock", "Clock", "0.1", "Clock for Plasma Mobile", KAboutLicense::GPL);
    KAboutData::setApplicationData(aboutData);

	auto *timeZoneModel = new TimeZoneSelectorModel();
	auto *timeZoneViewModel = new TimeZoneViewModel(timeZoneModel);
	auto *timeZoneFilterModel = new TimeZoneFilterModel(timeZoneModel);
    auto *alarmModel = new AlarmModel();
	engine.rootContext()->setContextProperty("timeZoneShowModel", timeZoneViewModel);
	engine.rootContext()->setContextProperty("timeZoneFilterModel", timeZoneFilterModel);
    engine.rootContext()->setContextProperty("alarmModel", alarmModel);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
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
