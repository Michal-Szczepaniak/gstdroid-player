/*
 * gst-droid
 *
 * Copyright (C) 2014 Mohammed Sameer <msameer@foolab.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "videoplayer.h"
#include <QQmlInfo>
#include <QTimer>
#include "renderer.h"
#include <QPainter>
#include <QMatrix4x4>
#include <cmath>

VideoPlayer::VideoPlayer(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    m_renderer(0),
    m_vsrc(0),
    m_asrc(0),
    m_vdecode(0),
    m_adecode(0),
    m_pipeline(0),
    m_bus(0),
    m_state(VideoPlayer::StateStopped),
    m_timer(new QTimer(this)),
    m_pos(0),
    m_created(false),
    m_audio_resource(this, AudioResourceQt::AudioResource::MediaType){

    m_timer->setSingleShot(false);
    m_timer->setInterval(500);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));

    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setSmooth(false);
    setAntialiasing(false);
    m_audio_resource.acquire();
}

VideoPlayer::~VideoPlayer() {
    stop();
    m_audio_resource.release();

    gst_element_set_state (m_pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pipeline));
}

void VideoPlayer::componentComplete() {
    QQuickPaintedItem::componentComplete();
}

void VideoPlayer::classBegin() {
    QQuickPaintedItem::classBegin();

    m_pipeline = gst_pipeline_new ("pipeline");
    loop = g_main_loop_new (NULL, FALSE);

    m_bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
    gst_bus_add_watch (m_bus, bus_call, this);
    gst_object_unref (m_bus);

    m_vsrc = gst_element_factory_make ("souphttpsrc", "VideoSrc");
    if (!m_vsrc) {
        qmlInfo(this) << "Failed to create VideoSrc";
        return;
    }

    m_asrc = gst_element_factory_make ("souphttpsrc", "AudioSrc");
    if (!m_asrc) {
        qmlInfo(this) << "Failed to create AudioSrc";
        return;
    }

    m_vdecode = gst_element_factory_make ("decodebin", "VideoDecoder");
    if (!m_vdecode) {
        qmlInfo(this) << "Failed to create VideoDecoder";
        return;
    }

    m_adecode = gst_element_factory_make ("decodebin", "AudioDecoder");
    if (!m_adecode) {
        qmlInfo(this) << "Failed to create AudioDecoder";
        return;
    }

    QString string = "https://download.blender.org/durian/trailer/sintel_trailer-480p.ogv";
    QByteArray array = string.toUtf8();
    g_object_set(m_vsrc, "location", array.constData(), NULL);

    string = "https://download.blender.org/durian/trailer/sintel_trailer-480p.ogv";
    array = string.toUtf8();
    g_object_set(m_asrc, "location", array.constData(), NULL);

//    g_signal_connect (G_OBJECT (m_pipeline), "notify::volume", G_CALLBACK (on_volume_changed), this);

    gst_bin_add_many (GST_BIN (m_pipeline), m_vsrc, /*m_asrc, */m_vdecode, /*m_adecode, */NULL);

    gst_element_link (m_vsrc, m_vdecode);
//    gst_element_link (m_asrc, m_adecode);

//    GstElement *elem = gst_element_factory_make("pulsesink", "VideoPlayerPulseSink");
//    qDebug() << elem;
//    g_signal_connect (m_adecode, "pad-added", G_CALLBACK (cb_new_pad), elem);
//    gst_bin_add(GST_BIN (m_pipeline), elem);

//    GstElement *elem = gst_element_factory_make("pulsesink", "VideoPlayerPulseSink");
//    g_signal_connect (m_adecode, "pad-added", G_CALLBACK (cb_new_pad), elem);
}

QUrl VideoPlayer::videoStreamUrl() const {
    return m_videoStreamUrl;
}

QUrl VideoPlayer::audioStreamUrl() const {
    return m_audioStreamUrl;
}

void VideoPlayer::setVideoStreamUrl(const QUrl& source) {
    if (m_videoStreamUrl != source) {
        m_videoStreamUrl = source;
        QString string = m_videoStreamUrl.toString();
        QByteArray array = string.toUtf8();
        g_object_set(m_vsrc, "location", array.constData(), NULL);
        emit videoStreamUrlChanged();
    }
}

void VideoPlayer::setAudioStreamUrl(const QUrl& source) {
    if (m_audioStreamUrl != source) {
        m_audioStreamUrl = source;
        QString string = m_audioStreamUrl.toString();
        QByteArray array = string.toUtf8();
        g_object_set(m_asrc, "location", array.constData(), NULL);
        emit audioStreamUrlChanged();
    }
}

qint64 VideoPlayer::duration() const {
    qint64 dur = 0;
    if (!gst_element_query_duration(m_pipeline, GST_FORMAT_TIME, &dur)) {
        return 0;
    }

    dur /= 1000000;

    return dur;
}

qint64 VideoPlayer::position() {
    qint64 pos = 0;
    if (!gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &pos)) {
        return m_pos;
    }

    pos /= 1000000;

    m_pos = pos;

    return pos;
}

void VideoPlayer::setPosition(qint64 position) {
    seek(position);
}

bool VideoPlayer::pause() {
    return setState(VideoPlayer::StatePaused);
}

bool VideoPlayer::play() {
    if (!m_renderer) {
        m_renderer = QtCamViewfinderRenderer::create(this);
        if (!m_renderer) {
            qmlInfo(this) << "Failed to create viewfinder renderer";
            return false;
        }


        QObject::connect(m_renderer, SIGNAL(updateRequested()), this, SLOT(updateRequested()));
    }

    m_renderer->resize(QSizeF(width(), height()));


    qDebug() << "AudioResource: " << m_audio_resource.isAcquired();

    gst_bin_add(GST_BIN (m_pipeline), m_renderer->sinkElement());
    g_signal_connect (m_vdecode, "pad-added", G_CALLBACK (cb_new_pad), m_renderer->sinkElement());

    return setState(VideoPlayer::StatePlaying);
}

bool VideoPlayer::seek(qint64 offset) {
    qint64 pos = offset;

    offset *= 1000000;

    GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);
    gboolean ret = gst_element_seek_simple (m_pipeline, GST_FORMAT_TIME,
                                            flags, offset);

    if (ret) {
        m_pos = pos;

        return TRUE;
    }

    return FALSE;
}

bool VideoPlayer::stop() {
    return setState(VideoPlayer::StateStopped);
}

void VideoPlayer::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);

    if (m_renderer) {
        m_renderer->resize(newGeometry.size());
    }
}


void VideoPlayer::paint(QPainter *painter) {
    painter->fillRect(contentsBoundingRect(), Qt::black);

    if (!m_renderer) {
        return;
    }

    bool needsNativePainting = m_renderer->needsNativePainting();

    if (needsNativePainting) {
        painter->beginNativePainting();
    }

    m_renderer->paint(QMatrix4x4(painter->combinedTransform()), painter->viewport());

    if (needsNativePainting) {
        painter->endNativePainting();
    }
}

VideoPlayer::State VideoPlayer::state() const {
    return m_state;
}

bool VideoPlayer::setPaused(int flags) {

    if (gst_element_set_state(m_pipeline, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
        qmlInfo(this) << "error setting pipeline to PAUSED";
        return false;
    }

    GstMessage *message =
            gst_bus_timed_pop_filtered(m_bus, GST_CLOCK_TIME_NONE,
                                       (GstMessageType)(GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_ERROR));

    if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR) {
        gst_message_unref (message);
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        return false;
    }

    gst_message_unref (message);
    return true;
}

bool VideoPlayer::setState(const VideoPlayer::State& state) {
    if (state == m_state) {
        return true;
    }

    if (state == VideoPlayer::StatePaused) {
        m_timer->stop();

        if (gst_element_set_state(m_pipeline, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "error setting pipeline to PAUSED";
            return false;
        }

        GstState st;
        if (gst_element_get_state(m_pipeline, &st, NULL, GST_CLOCK_TIME_NONE)
                == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "setting pipeline to PAUSED failed";
            return false;
        }

        if (st != GST_STATE_PAUSED) {
            qmlInfo(this) << "pipeline failed to transition to to PAUSED state";
            return false;
        }

        m_state = state;
        emit stateChanged();

        return true;
    }
    else if (state == VideoPlayer::StatePlaying) {
        if (gst_element_set_state(m_pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "error setting pipeline to PLAYING";
            return false;
        }

        m_state = state;
        emit stateChanged();

        emit durationChanged();
        emit positionChanged();

        m_timer->start();
        return true;
    }
    else {
        m_timer->stop();
        m_pos = 0;

        delete m_renderer;
        m_renderer = 0;

        if (gst_element_set_state(m_pipeline, GST_STATE_NULL) == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "error setting pipeline to NULL";
            return false;
        }

        GstState st;
        if (gst_element_get_state(m_pipeline, &st, NULL, GST_CLOCK_TIME_NONE)
                == GST_STATE_CHANGE_FAILURE) {
            qmlInfo(this) << "setting pipeline to NULL failed";
            return false;
        }

        if (st != GST_STATE_NULL) {
            qmlInfo(this) << "pipeline failed to transition to to NULL state";
            return false;
        }

        m_state = state;
        emit stateChanged();

        emit durationChanged();
        emit positionChanged();

        return true;
    }
}

gboolean VideoPlayer::bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    Q_UNUSED(bus);

    VideoPlayer *that = (VideoPlayer *) data;

    gchar *debug = NULL;
    GError *err = NULL;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        that->stop();
        break;

    case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug);
        qWarning() << "Error" << err->message << err->code;

//        emit that->error(QString::fromUtf8(err->message), (int)err->code, debug);
//        that->stop();

        if (err) {
            g_error_free (err);
        }

        if (debug) {
            g_free (debug);
        }

        break;

    default:
        break;
    }

    return TRUE;
}

void VideoPlayer::cb_new_pad (GstElement *element, GstPad *pad, gpointer data)
{
  gchar *name;
  GstElement *other = (GstElement*)data;

  name = gst_pad_get_name (pad);
  g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
  g_free (name);

  g_print ("element %s will be linked to %s\n",
           gst_element_get_name(element),
           gst_element_get_name(other));
  gst_element_link(element, other);
}

void VideoPlayer::updateRequested() {
    update();
}

quint32 VideoPlayer::volume() {
    double vol = 1.0;
    g_object_get (m_pipeline, "volume", &vol, NULL);

    qint32 res = (int)round(vol * 100.0);

    return res;
}

void VideoPlayer::setVolume(quint32 volume) {
    if (VideoPlayer::volume() != volume) {
        double vol = volume / 100.0;
        g_object_set (m_pipeline, "volume", vol, NULL);
        emit volumeChanged();
    }
}

void VideoPlayer::on_volume_changed(GObject *object, GParamSpec *pspec, gpointer user_data) {
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    VideoPlayer *player = (VideoPlayer *) user_data;

    QMetaObject::invokeMethod(player, "volumeChanged", Qt::QueuedConnection);
}
