#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
