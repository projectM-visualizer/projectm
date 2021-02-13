/* Copyright (C) 2006 Tim-Philipp Müller <tim centricular net>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gst-app.h"

static void
handle_file_or_directory (const gchar * filename)
{
  GError *err = NULL;
  GDir *dir;
  gchar *uri;

  if ((dir = g_dir_open (filename, 0, NULL))) {
    const gchar *entry;

    while ((entry = g_dir_read_name (dir))) {
      gchar *path;

      path = g_strconcat (filename, G_DIR_SEPARATOR_S, entry, NULL);
      handle_file_or_directory (path);
      g_free (path);
    }

    g_dir_close (dir);
    return;
  }

  if (g_path_is_absolute (filename)) {
    uri = g_filename_to_uri (filename, NULL, &err);
  } else {
    gchar *curdir, *absolute_path;

    curdir = g_get_current_dir ();
    absolute_path = g_strconcat ( curdir, G_DIR_SEPARATOR_S, filename, NULL);
    uri = g_filename_to_uri (absolute_path, NULL, &err);
    g_free (absolute_path);
    g_free (curdir);
  }

  if (uri) {
    /* great, we have a proper file:// URI, let's play it! */
    play_uri (uri);
  } else {
    g_warning ("Failed to convert filename '%s' to URI: %s", filename,
        err->message);
    g_error_free (err);
  }

  g_free (uri);
}

int
main (int argc, char *argv[])
{
  gchar **filenames = NULL;
  const GOptionEntry entries[] = {
    /* you can add your won command line options here */
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames,
      "Special option that collects any remaining arguments for us" },
    { NULL, }
  };
  GOptionContext *ctx;
  GError *err = NULL;
  gint i, num;

  /* Before calling any GLib or GStreamer function, we must initialise
   * the GLib threading system */
  if (!g_thread_supported())
    g_thread_init (NULL);

  ctx = g_option_context_new ("[FILE1] [FILE2] ...");
  g_option_context_add_group (ctx, gst_init_get_option_group ());
  g_option_context_add_main_entries (ctx, entries, NULL);

  if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
    g_print ("Error initializing: %s\n", GST_STR_NULL (err->message));
    return -1;
  }
  g_option_context_free (ctx);

  if (filenames == NULL || *filenames == NULL) {
    g_print ("Please specify a file to play\n\n");
    return -1;
  }



  num = g_strv_length (filenames);

  for (i = 0; i < num; ++i) {
    handle_file_or_directory (filenames[i]);
  }

  g_strfreev (filenames);

  return 0;
}
