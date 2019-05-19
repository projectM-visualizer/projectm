/*
 * 2019 Mischa Spiegelmock
 *
 */

#ifndef _GST_PROJECTMVIZ_H_
#define _GST_PROJECTMVIZ_H_

#include "projectM.hpp"
#include <gst/audio/gstaudiosink.h>

G_BEGIN_DECLS

#define GST_TYPE_PROJECTMVIZ (gst_projectmviz_get_type())
#define GST_PROJECTMVIZ(obj)                                                   \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_PROJECTMVIZ, GstProjectmviz))
#define GST_PROJECTMVIZ_CLASS(klass)                                           \
  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_PROJECTMVIZ, GstProjectmvizClass))
#define GST_IS_PROJECTMVIZ(obj)                                                \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_PROJECTMVIZ))
#define GST_IS_PROJECTMVIZ_CLASS(obj)                                          \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_PROJECTMVIZ))

typedef struct _GstProjectmviz GstProjectmviz;
typedef struct _GstProjectmvizClass GstProjectmvizClass;

struct _GstProjectmviz {
  GstAudioSink base_projectmviz;

  gint channels;

  gint width;
  gint height;

  projectM *pM;
};

struct _GstProjectmvizClass {
  GstAudioSinkClass base_projectmviz_class;
};

GType gst_projectmviz_get_type(void);

G_END_DECLS

#endif
