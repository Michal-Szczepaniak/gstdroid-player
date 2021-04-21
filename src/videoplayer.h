// -*- c++ -*-

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

#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QQuickPaintedItem>
#include <AudioResourceQt>
#include <gst/gst.h>

class QtCamViewfinderRenderer;

class VideoPlayer : public QQuickPaintedItem {
  Q_OBJECT

  Q_PROPERTY(QUrl videoStreamUrl READ videoStreamUrl WRITE setVideoStreamUrl NOTIFY videoStreamUrlChanged)
  Q_PROPERTY(QUrl audioStreamUrl READ audioStreamUrl WRITE setAudioStreamUrl NOTIFY audioStreamUrlChanged)
  Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
  Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(State state READ state NOTIFY stateChanged)
  Q_ENUMS(State)
  Q_PROPERTY(quint32 volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
  VideoPlayer(QQuickItem *parent = 0);
  ~VideoPlayer();

  virtual void componentComplete();
  virtual void classBegin();

  void paint(QPainter *painter);

  QUrl videoStreamUrl() const;
  QUrl audioStreamUrl() const;
  void setVideoStreamUrl(const QUrl& videoStreamUrl);
  void setAudioStreamUrl(const QUrl& audioStreamUrl);

  qint64 duration() const;
  qint64 position();
  void setPosition(qint64 position);

  Q_INVOKABLE bool pause();
  Q_INVOKABLE bool play();
  Q_INVOKABLE bool seek(qint64 offset);
  Q_INVOKABLE bool stop();

  typedef enum {
    StateStopped,
    StatePaused,
    StatePlaying,
  } State;

  State state() const;

  quint32 volume();
  void setVolume(quint32 volume);

signals:
  void stateChanged();
  void sourceChanged();
  void durationChanged();
  void positionChanged();
  void error(const QString& message, int code, const QString& debug);
  void videoStreamUrlChanged();
  void audioStreamUrlChanged();
  void volumeChanged();

protected:
  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private slots:
  void updateRequested();

private:
  static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
  static void cb_new_pad (GstElement *element, GstPad *pad, gpointer data);
  static void on_volume_changed(GObject *object, GParamSpec *pspec, gpointer user_data);

  bool setState(const State& state);
  bool setPaused(int flags);

  QtCamViewfinderRenderer *m_renderer;
  AudioResourceQt::AudioResource m_audio_resource;

  GstBus *m_bus;
  GMainLoop *loop;

  GstElement *m_pipeline;
  GstElement *m_vsrc;
  GstElement *m_asrc;
  GstElement *m_vdecode;
  GstElement *m_adecode;

  QUrl m_videoStreamUrl;
  QUrl m_audioStreamUrl;
  State m_state;
  QTimer *m_timer;
  qint64 m_pos;
  bool m_created;
};

#endif /* VIDEO_PLAYER_H */
