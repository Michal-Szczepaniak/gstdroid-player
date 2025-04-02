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

    view->rootContext()->setContextProperty("_videoSource", QUrl::fromUserInput(app->arguments().at(1)));
    view->rootContext()->setContextProperty("_audioSource", QUrl::fromUserInput(app->arguments().at(2)));

    qmlRegisterType<VideoPlayer>("VideoPlayer", 1, 0, "VideoPlayer");

    view->setSource(SailfishApp::pathTo("qml/gstdroid-player.qml"));
    view->show();

    return app->exec();
}
