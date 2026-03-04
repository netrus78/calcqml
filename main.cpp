#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "calculator.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Calculator>("Calc", 1, 0, "Calculator");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/calcqml/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
