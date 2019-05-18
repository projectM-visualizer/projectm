/* GstProjectM
 * Copyright (C) 2019 FIXME <fixme@example.com>
 * Copyright (C) 2010 Entropy Wave Inc
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "element/gstprojectmviz.hpp"
#include <gst/gst.h>
#include <stdlib.h>

#define GETTEXT_PACKAGE "projectM"

typedef struct _GstProjectM GstProjectM;
struct _GstProjectM {
  GstElement *pipeline;
  GstBus *bus;
  GMainLoop *main_loop;

  GstElement *source_element;
  GstElement *sink_element;

  gboolean paused_for_buffering;
  guint timer_id;
};

GstProjectM *gst_projectM_new(void);
void gst_projectM_free(GstProjectM *projectM);
void gst_projectM_create_pipeline(GstProjectM *projectM);
void gst_projectM_create_pipeline_playbin(GstProjectM *projectM,
                                          const char *uri);
void gst_projectM_start(GstProjectM *projectM);
void gst_projectM_stop(GstProjectM *projectM);

static gboolean gst_projectM_handle_message(GstBus *bus, GstMessage *message,
                                            gpointer data);
static gboolean onesecond_timer(gpointer priv);

gboolean verbose;

static GOptionEntry entries[] = {
    {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL},

    {NULL}

};

int main(int argc, char *argv[]) {
  GError *error = NULL;
  GOptionContext *context;
  GstProjectM *projectM;
  GMainLoop *main_loop;

  context = g_option_context_new("- FIXME");
  g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);
  g_option_context_add_group(context, gst_init_get_option_group());
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_print("option parsing failed: %s\n", error->message);
    g_clear_error(&error);
    g_option_context_free(context);
    exit(1);
  }
  g_option_context_free(context);

  projectM = gst_projectM_new();

  if (argc > 1) {
    gchar *uri;
    if (gst_uri_is_valid(argv[1])) {
      uri = g_strdup(argv[1]);
    } else {
      uri = g_filename_to_uri(argv[1], NULL, NULL);
    }
    gst_projectM_create_pipeline_playbin(projectM, uri);
    g_free(uri);
  } else {
    gst_projectM_create_pipeline(projectM);
  }

  gst_projectM_start(projectM);

  main_loop = g_main_loop_new(NULL, TRUE);
  projectM->main_loop = main_loop;

  g_main_loop_run(main_loop);

  exit(0);
}

GstProjectM *gst_projectM_new(void) {
  GstProjectM *projectM;

  projectM = g_new0(GstProjectM, 1);

  return projectM;
}

void gst_projectM_free(GstProjectM *projectM) {
  if (projectM->source_element) {
    gst_object_unref(projectM->source_element);
    projectM->source_element = NULL;
  }
  if (projectM->sink_element) {
    gst_object_unref(projectM->sink_element);
    projectM->sink_element = NULL;
  }

  if (projectM->pipeline) {
    gst_element_set_state(projectM->pipeline, GST_STATE_NULL);
    gst_object_unref(projectM->pipeline);
    projectM->pipeline = NULL;
  }
  g_free(projectM);
}

void gst_projectM_create_pipeline_playbin(GstProjectM *projectM,
                                          const char *uri) {
  GstElement *pipeline;
  GError *error = NULL;

  pipeline = gst_pipeline_new(NULL);

  // fake audio source
  GstElement *audio_src_element = gst_element_factory_make("audiotestsrc", "audio-source");
  gst_bin_add(GST_BIN(pipeline), audio_src_element);

  /* gst_bin_add(GST_BIN(pipeline), gst_element_factory_make("playbin", "source")); */

  if (error) {
    g_print("pipeline parsing error: %s\n", error->message);
    g_clear_error(&error);
    gst_object_unref(pipeline);
    return;
  }

  projectM->pipeline = pipeline;

  gst_pipeline_set_auto_flush_bus(GST_PIPELINE(pipeline), FALSE);
  projectM->bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_watch(projectM->bus, gst_projectM_handle_message, projectM);

  projectM->source_element = audio_src_element;
  g_print("source_element is %p\n", projectM->source_element);

  // create projectM element and add it as the audio sink
  GstElement *pM_ele = gst_element_factory_make("projectmviz", "projectM-visualizer");
  if (!pM_ele) {
      g_printerr("Could not create projectM element\n");
      G_BREAKPOINT();
      return;
  }
  gst_bin_add(GST_BIN(pipeline), pM_ele);
  gst_element_link(audio_src_element, pM_ele);
  g_print("linked audio source to projectM\n");

  g_print("setting uri to %s\n", uri);
  g_object_set(projectM->source_element, "samplesperbuffer", 1600, NULL);
  g_object_set(projectM->source_element, "num-buffers", 100, NULL);
}

void gst_projectM_create_pipeline(GstProjectM *projectM) {
  GString *pipe_desc;
  GstElement *pipeline;
  GError *error = NULL;



  pipe_desc = g_string_new("");

  g_string_append(pipe_desc, "videotestsrc name=source num-buffers=100 ! ");
  g_string_append(pipe_desc, "timeoverlay ! ");
  g_string_append(pipe_desc, "xvimagesink name=sink ");
  g_string_append(pipe_desc,
                  "audiotestsrc samplesperbuffer=1600 num-buffers=100 ! ");
  g_string_append(pipe_desc, "alsasink ");

  if (verbose)
    g_print("pipeline: %s\n", pipe_desc->str);

  pipeline = (GstElement *)gst_parse_launch(pipe_desc->str, &error);
  g_string_free(pipe_desc, FALSE);

  if (error) {
    g_print("pipeline parsing error: %s\n", error->message);
    g_clear_error(&error);
    gst_object_unref(pipeline);
    return;
  }

  projectM->pipeline = pipeline;

  gst_pipeline_set_auto_flush_bus(GST_PIPELINE(pipeline), FALSE);
  projectM->bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_watch(projectM->bus, gst_projectM_handle_message, projectM);

  projectM->source_element = gst_bin_get_by_name(GST_BIN(pipeline), "source");
  projectM->sink_element = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

}

void gst_projectM_start(GstProjectM *projectM) {
  gst_element_set_state(projectM->pipeline, GST_STATE_READY);

  projectM->timer_id = g_timeout_add(1000, onesecond_timer, projectM);
}

void gst_projectM_stop(GstProjectM *projectM) {
  gst_element_set_state(projectM->pipeline, GST_STATE_NULL);

  g_source_remove(projectM->timer_id);
}

static void gst_projectM_handle_eos(GstProjectM *projectM) {
  gst_projectM_stop(projectM);
}

static void gst_projectM_handle_error(GstProjectM *projectM, GError *error,
                                      const char *debug) {
  g_print("error: %s\n", error->message);
  gst_projectM_stop(projectM);
}

static void gst_projectM_handle_warning(GstProjectM *projectM, GError *error,
                                        const char *debug) {
  g_print("warning: %s\n", error->message);
}

static void gst_projectM_handle_info(GstProjectM *projectM, GError *error,
                                     const char *debug) {
  g_print("info: %s\n", error->message);
}

static void gst_projectM_handle_null_to_ready(GstProjectM *projectM) {
  gst_element_set_state(projectM->pipeline, GST_STATE_PAUSED);
}

static void gst_projectM_handle_ready_to_paused(GstProjectM *projectM) {
  if (!projectM->paused_for_buffering) {
    gst_element_set_state(projectM->pipeline, GST_STATE_PLAYING);
  }
}

static void gst_projectM_handle_paused_to_playing(GstProjectM *projectM) {}

static void gst_projectM_handle_playing_to_paused(GstProjectM *projectM) {}

static void gst_projectM_handle_paused_to_ready(GstProjectM *projectM) {}

static void gst_projectM_handle_ready_to_null(GstProjectM *projectM) {
  g_main_loop_quit(projectM->main_loop);
}

static gboolean gst_projectM_handle_message(GstBus *bus, GstMessage *message,
                                            gpointer data) {
  GstProjectM *projectM = (GstProjectM *)data;

  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_EOS:
    gst_projectM_handle_eos(projectM);
    break;
  case GST_MESSAGE_ERROR: {
    GError *error = NULL;
    gchar *debug;

    gst_message_parse_error(message, &error, &debug);
    gst_projectM_handle_error(projectM, error, debug);
    g_clear_error(&error);
  } break;
  case GST_MESSAGE_WARNING: {
    GError *error = NULL;
    gchar *debug;

    gst_message_parse_warning(message, &error, &debug);
    gst_projectM_handle_warning(projectM, error, debug);
    g_clear_error(&error);
  } break;
  case GST_MESSAGE_INFO: {
    GError *error = NULL;
    gchar *debug;

    gst_message_parse_info(message, &error, &debug);
    gst_projectM_handle_info(projectM, error, debug);
    g_clear_error(&error);
  } break;
  case GST_MESSAGE_TAG: {
    GstTagList *tag_list;

    gst_message_parse_tag(message, &tag_list);
    if (verbose)
      g_print("tag\n");
  } break;
  case GST_MESSAGE_STATE_CHANGED: {
    GstState oldstate, newstate, pending;

    gst_message_parse_state_changed(message, &oldstate, &newstate, &pending);
    if (GST_ELEMENT(message->src) == projectM->pipeline) {
      if (verbose)
        g_print("state change from %s to %s\n",
                gst_element_state_get_name(oldstate),
                gst_element_state_get_name(newstate));
      switch (GST_STATE_TRANSITION(oldstate, newstate)) {
      case GST_STATE_CHANGE_NULL_TO_READY:
        gst_projectM_handle_null_to_ready(projectM);
        break;
      case GST_STATE_CHANGE_READY_TO_PAUSED:
        gst_projectM_handle_ready_to_paused(projectM);
        break;
      case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
        gst_projectM_handle_paused_to_playing(projectM);
        break;
      case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
        gst_projectM_handle_playing_to_paused(projectM);
        break;
      case GST_STATE_CHANGE_PAUSED_TO_READY:
        gst_projectM_handle_paused_to_ready(projectM);
        break;
      case GST_STATE_CHANGE_READY_TO_NULL:
        gst_projectM_handle_ready_to_null(projectM);
        break;
      default:
        if (verbose)
          g_print("unknown state change from %s to %s\n",
                  gst_element_state_get_name(oldstate),
                  gst_element_state_get_name(newstate));
      }
    }
  } break;
  case GST_MESSAGE_BUFFERING: {
    int percent;
    gst_message_parse_buffering(message, &percent);
    // g_print("buffering %d\n", percent);
    if (!projectM->paused_for_buffering && percent < 100) {
      g_print("pausing for buffing\n");
      projectM->paused_for_buffering = TRUE;
      gst_element_set_state(projectM->pipeline, GST_STATE_PAUSED);
    } else if (projectM->paused_for_buffering && percent == 100) {
      g_print("unpausing for buffing\n");
      projectM->paused_for_buffering = FALSE;
      gst_element_set_state(projectM->pipeline, GST_STATE_PLAYING);
    }
  } break;
  case GST_MESSAGE_STATE_DIRTY:
  case GST_MESSAGE_CLOCK_PROVIDE:
  case GST_MESSAGE_CLOCK_LOST:
  case GST_MESSAGE_NEW_CLOCK:
  case GST_MESSAGE_STRUCTURE_CHANGE:
  case GST_MESSAGE_STREAM_STATUS:
    break;
  case GST_MESSAGE_STEP_DONE:
  case GST_MESSAGE_APPLICATION:
  case GST_MESSAGE_ELEMENT:
  case GST_MESSAGE_SEGMENT_START:
  case GST_MESSAGE_SEGMENT_DONE:
  case GST_MESSAGE_DURATION:
  case GST_MESSAGE_LATENCY:
  case GST_MESSAGE_ASYNC_START:
  case GST_MESSAGE_ASYNC_DONE:
  case GST_MESSAGE_REQUEST_STATE:
  case GST_MESSAGE_STEP_START:
  case GST_MESSAGE_QOS:
  default:
    if (verbose) {
      g_print("message: %s\n", GST_MESSAGE_TYPE_NAME(message));
    }
    break;
  }

  return TRUE;
}

static gboolean onesecond_timer(gpointer priv) {
  // GstProjectM *projectM = (GstProjectM *)priv;

  g_print(".\n");

  return TRUE;
}

/* helper functions */

#if 0
gboolean
have_element (const gchar * element_name)
{
  GstPluginFeature *feature;

  feature = gst_default_registry_find_feature (element_name,
      GST_TYPE_ELEMENT_FACTORY);
  if (feature) {
    g_object_unref (feature);
    return TRUE;
  }
  return FALSE;
}
#endif
