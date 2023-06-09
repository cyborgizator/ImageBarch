#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "imagefilesmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<ImageFilesModel>("my.customcomponents", 1, 0, "ImageFilesModel");

    QQmlApplicationEngine engine;

    if (argc > 1)
    {
        QVariant path = QString(argv[1]);
        engine.rootContext()->setContextProperty("ImagesDirectory", path);
    }

    const QUrl url(u"qrc:/ImageBarch/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
