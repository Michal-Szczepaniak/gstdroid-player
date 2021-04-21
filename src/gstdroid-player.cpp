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

    QString source = "https://r1---sn-n3tf5gx-g1xl.googlevideo.com/videoplayback?expire=1619050215&ei=h2qAYIyOGcm8yAXSjKSADw&ip=91.189.216.255&id=o-AGQLXlwmCWFpu_51gAqn1oFLfGuYq0EXy_Un7ex1TTte&itag=315&aitags=133%2C134%2C135%2C136%2C160%2C242%2C243%2C244%2C247%2C278%2C298%2C299%2C302%2C303%2C308%2C315%2C330%2C331%2C332%2C333%2C334%2C335%2C336%2C337%2C394%2C395%2C396%2C397%2C398%2C399%2C400%2C401%2C694%2C695%2C696%2C697%2C698%2C699%2C700%2C701&source=youtube&requiressl=yes&mh=NH&mm=31%2C29&mn=sn-n3tf5gx-g1xl%2Csn-u2oxu-f5fe7&ms=au%2Crdu&mv=m&mvi=1&pl=21&initcwndbps=1132500&vprv=1&mime=video%2Fwebm&ns=Ng8viWKsUl0BcQO70hGRrUwF&gir=yes&clen=1003049196&dur=313.780&lmt=1617458240598436&mt=1619028301&fvip=6&keepalive=yes&fexp=24001373%2C24007246&c=WEB&txp=5532232&n=qbdtiDi7_9G3bl9L&sparams=expire%2Cei%2Cip%2Cid%2Caitags%2Csource%2Crequiressl%2Cvprv%2Cmime%2Cns%2Cgir%2Cclen%2Cdur%2Clmt&sig=AOq0QJ8wRQIgFF3yUaHmnRUzQWri3Iue2Rq0JFTevp3rc1MYpw6AGUcCIQCPji1Hh1WdylXkpYWJY8ns3Ghy8bEjmFJn_sqpug6GaA%3D%3D&lsparams=mh%2Cmm%2Cmn%2Cms%2Cmv%2Cmvi%2Cpl%2Cinitcwndbps&lsig=AG3C_xAwRgIhAPiCwqW3sKXjWrZMJ3ONSTLm-Hdv4mmaYhq4D7tk1s4LAiEAuXX0YBjj4K9_SClFL9Pqdt9SmtRpCsg4kcvnpBpvIV4%3D";
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
