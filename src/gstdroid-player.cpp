#include <QtQuick>
#include <sailfishapp.h>
#include <unistd.h>
#include "player.h"
#include "quickviewhelper.h"

QQuickView *view;

int main(int argc, char *argv[])
{
    gst_init (&argc, &argv);

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QSharedPointer<QQuickView> view(SailfishApp::createView());

    QuickViewHelper::setView(view.data());

    qmlRegisterType<VideoPlayer>("VideoPlayer", 1, 0, "VideoPlayer");

    view->setSource(SailfishApp::pathTo("qml/gstdroid-player.qml"));
    view->show();

    return app->exec();
}
