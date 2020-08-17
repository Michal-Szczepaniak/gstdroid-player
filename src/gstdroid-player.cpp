#include <QtQuick>
#include <sailfishapp.h>
#include <unistd.h>
#include "player.h"

int main(int argc, char *argv[])
{
    gst_init(NULL,NULL);

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));

    QQuickView *view = SailfishApp::createView();
    view->setPersistentOpenGLContext(true);
    view->setPersistentSceneGraph(true);

    QString source = "https://download.blender.org/durian/trailer/sintel_trailer-480p.ogv";
    if(app->arguments().count() > 1)
    {
        source = app->arguments().at(1);
    }

    view->rootContext()->setContextProperty("_source", QUrl::fromUserInput(source));

    qmlRegisterType<VideoPlayer>("VideoPlayer", 1, 0, "VideoPlayer");

    view->setSource(SailfishApp::pathTo("qml/gstdroid-player.qml"));
    view->show();

    int result = app->exec();
     gst_deinit ();
     return result;
}
