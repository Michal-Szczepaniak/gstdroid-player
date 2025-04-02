TARGET = gstdroid-player

CONFIG += c++2a link_pkgconfig sailfishapp

QMAKE_CXXFLAGS += -std=c++2a

PKGCONFIG += \
    gstreamer-1.0 \
    gstreamer-player-1.0 \
    gstreamer-video-1.0 \
    glib-2.0 \
    nemo-gstreamer-interfaces-1.0 \
    Qt5OpenGL \
    Qt5OpenGLExtensions \
    egl \
    audioresource-qt \

INCLUDEPATH += \
    /usr/include/gstreamer-1.0 \
    /usr/include/audioresource-qt

SOURCES += src/gstdroid-player.cpp \
    src/player.cpp \
    src/renderernemo.cpp

DISTFILES += qml/gstdroid-player.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    rpm/gstdroid-player.spec \
    gstdroid-player.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128

# to disable building translations every time, comment out the
# following CONFIG line
# CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/gstvideotest-de.ts

HEADERS += \
    src/player.h \
    src/renderernemo.h \
    src/quickviewhelper.h
