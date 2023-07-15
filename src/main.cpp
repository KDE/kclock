#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <klocalizedcontext.h>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine *engine = new QQmlApplicationEngine();

    engine->rootContext()->setContextObject(new KLocalizedContext(engine));

    engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.kclock")));
    return app.exec();
}
