/*
 * 2019 Mischa Spiegelmock
 */
/**
 * SECTION:element-gstprojectmviz
 *
 * The projectmviz element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! projectmviz ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstprojectmviz.hpp"
#include <gst/audio/gstaudiosink.h>
#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC(gst_projectmviz_debug_category);
#define GST_CAT_DEFAULT gst_projectmviz_debug_category

#define DEFAULT_WIDTH 1080
#define DEFAULT_HEIGHT 720

projectM::Settings pmSettings;
pmSettings.windowWidth = DEFAULT_WIDTH;
pmSettings.windowHeight = DEFAULT_HEIGHT;

/* prototypes */

static void gst_projectmviz_set_property(GObject *object, guint property_id,
                                         const GValue *value,
                                         GParamSpec *pspec);
static void gst_projectmviz_get_property(GObject *object, guint property_id,
                                         GValue *value, GParamSpec *pspec);
static void gst_projectmviz_dispose(GObject *object);
static void gst_projectmviz_finalize(GObject *object);

static gboolean gst_projectmviz_open(GstAudioSink *sink);
static gboolean gst_projectmviz_prepare(GstAudioSink *sink,
                                        GstAudioRingBufferSpec *spec);
static gboolean gst_projectmviz_unprepare(GstAudioSink *sink);
static gboolean gst_projectmviz_close(GstAudioSink *sink);
static gint gst_projectmviz_write(GstAudioSink *sink, gpointer data,
                                  guint length);
static guint gst_projectmviz_delay(GstAudioSink *sink);
static void gst_projectmviz_reset(GstAudioSink *sink);

enum { PROP_0 };

/* pad templates */

/* FIXME add/remove the formats that you want to support */
static GstStaticPadTemplate gst_projectmviz_sink_template =
    GST_STATIC_PAD_TEMPLATE(
        "sink", GST_PAD_SINK, GST_PAD_ALWAYS,
        GST_STATIC_CAPS("audio/x-raw,format=S16LE,rate=[48000],"
                        "channels=[1],layout=interleaved"));

/* class initialization */

G_DEFINE_TYPE_WITH_CODE(
    GstProjectmviz, gst_projectmviz, GST_TYPE_AUDIO_SINK,
    GST_DEBUG_CATEGORY_INIT(gst_projectmviz_debug_category, "projectmviz", 0,
                            "debug category for projectmviz element"));

static void gst_projectmviz_class_init(GstProjectmvizClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GstAudioSinkClass *audio_sink_class = GST_AUDIO_SINK_CLASS(klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass),
                                            &gst_projectmviz_sink_template);

  gst_element_class_set_static_metadata(
      GST_ELEMENT_CLASS(klass), "ProjectM Visualizer", "Generic",
      "Visualizes audio input", "<mischa@mvstg.biz>");

  gobject_class->set_property = gst_projectmviz_set_property;
  gobject_class->get_property = gst_projectmviz_get_property;
  gobject_class->dispose = gst_projectmviz_dispose;
  gobject_class->finalize = gst_projectmviz_finalize;
  audio_sink_class->open = GST_DEBUG_FUNCPTR(gst_projectmviz_open);
  audio_sink_class->prepare = GST_DEBUG_FUNCPTR(gst_projectmviz_prepare);
  audio_sink_class->unprepare = GST_DEBUG_FUNCPTR(gst_projectmviz_unprepare);
  audio_sink_class->close = GST_DEBUG_FUNCPTR(gst_projectmviz_close);
  audio_sink_class->write = GST_DEBUG_FUNCPTR(gst_projectmviz_write);
  audio_sink_class->delay = GST_DEBUG_FUNCPTR(gst_projectmviz_delay);
  audio_sink_class->reset = GST_DEBUG_FUNCPTR(gst_projectmviz_reset);
}

static void gst_projectmviz_init(GstProjectmviz *pmviz) {
  pmviz->width = DEFAULT_WIDTH;
  pmviz->height = DEFAULT_HEIGHT;
  pmviz->channels = 0;

  pmviz->pM = new projectM();
}

void gst_projectmviz_set_property(GObject *object, guint property_id,
                                  const GValue *value, GParamSpec *pspec) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(object);

  GST_DEBUG_OBJECT(projectmviz, "set_property");

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

void gst_projectmviz_get_property(GObject *object, guint property_id,
                                  GValue *value, GParamSpec *pspec) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(object);

  GST_DEBUG_OBJECT(projectmviz, "get_property");

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

void gst_projectmviz_dispose(GObject *object) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(object);

  GST_DEBUG_OBJECT(projectmviz, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS(gst_projectmviz_parent_class)->dispose(object);
}

void gst_projectmviz_finalize(GObject *object) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(object);

  GST_DEBUG_OBJECT(projectmviz, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS(gst_projectmviz_parent_class)->finalize(object);
}

/* open the device with given specs */
static gboolean gst_projectmviz_open(GstAudioSink *sink) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "open");

  return TRUE;
}

/* prepare resources and state to operate with the given specs */
static gboolean gst_projectmviz_prepare(GstAudioSink *sink,
                                        GstAudioRingBufferSpec *spec) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "prepare");

  return TRUE;
}

/* undo anything that was done in prepare() */
static gboolean gst_projectmviz_unprepare(GstAudioSink *sink) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "unprepare");

  return TRUE;
}

/* close the device */
static gboolean gst_projectmviz_close(GstAudioSink *sink) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "close");

  return TRUE;
}

/* write samples to the device */
static gint gst_projectmviz_write(GstAudioSink *sink, gpointer data,
                                  guint length) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "write");

  return 0;
}

/* get number of samples queued in the device */
static guint gst_projectmviz_delay(GstAudioSink *sink) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "delay");

  return 0;
}

/* reset the audio device, unblock from a write */
static void gst_projectmviz_reset(GstAudioSink *sink) {
  GstProjectmviz *projectmviz = GST_PROJECTMVIZ(sink);

  GST_DEBUG_OBJECT(projectmviz, "reset");
}

static gboolean plugin_init(GstPlugin *plugin) {

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register(plugin, "projectmviz", GST_RANK_NONE,
                              GST_TYPE_PROJECTMVIZ);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.1"
#endif
#ifndef PACKAGE
#define PACKAGE "projectmviz"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "projectmviz"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, projectmviz,
                  "FIXME plugin description", plugin_init, VERSION, "LGPL",
                  PACKAGE_NAME, GST_PACKAGE_ORIGIN)
