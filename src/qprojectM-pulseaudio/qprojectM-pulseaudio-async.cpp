
 /* $Id: pacat.c 1426 2007-02-13 15:35:19Z ossman $ */
 
 /***
   This file is part of PulseAudio.
 
   Copyright 2004-2006 Lennart Poettering
   Copyright 2006 Pierre Ossman <ossman@cendio.se> for Cendio AB
 
   PulseAudio is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.
 
   PulseAudio is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with PulseAudio; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.
 ***/
 
 #ifdef HAVE_CONFIG_H
 #include <config.h>
 #endif
 
 #include <signal.h>
 #include <string.h>
 #include <errno.h>
 #include <unistd.h>
 #include <assert.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <getopt.h>
 #include <fcntl.h>
 
extern "C" {
#include <pulse/pulseaudio.h>
}

 #define TIME_EVENT_USEC 50000
 
 #if PA_API_VERSION < 9
 #error Invalid PulseAudio API version
 #endif
 
 static enum { RECORD, PLAYBACK } mode = PLAYBACK;
 
 static pa_context *context = NULL;
 static pa_stream *stream = NULL;
 static pa_mainloop_api *mainloop_api = NULL;
 
 static float *buffer = NULL;
 static size_t buffer_length = 0, buffer_index = 0;
 
 static pa_io_event* stdio_event = NULL;
 
 static char *stream_name = NULL, *client_name = NULL, *device = NULL;
 
 static int verbose = 0;
 static pa_volume_t volume = PA_VOLUME_NORM;
 
 
 static pa_channel_map channel_map;
 static int channel_map_set = 0;
 static pa_sample_spec sample_spec ;
 /* A shortcut for terminating the application */
 static void quit(int ret) {
     assert(mainloop_api);
     mainloop_api->quit(mainloop_api, ret);
 }
 
 /* Write some data to the stream */
 static void do_stream_write(size_t length) {
     size_t l;
     assert(length);
 
     if (!buffer || !buffer_length)
         return;
 
     l = length;
     if (l > buffer_length)
         l = buffer_length;
 
     if (pa_stream_write(stream, (uint8_t*) buffer + buffer_index, l, NULL, 0, PA_SEEK_RELATIVE) < 0) {
         fprintf(stderr, "pa_stream_write() failed: %s\n", pa_strerror(pa_context_errno(context)));
         quit(1);
         return;
     }
 
     buffer_length -= l;
     buffer_index += l;
 
     if (!buffer_length) {
         pa_xfree(buffer);
         buffer = NULL;
         buffer_index = buffer_length = 0;
     }
 }
 
 /* This is called whenever new data may be written to the stream */
 static void stream_write_callback(pa_stream *s, size_t length, void *userdata) {
     assert(s && length);
 
     if (stdio_event)
         mainloop_api->io_enable(stdio_event, PA_IO_EVENT_INPUT);
 
     if (!buffer)
         return;
 
     do_stream_write(length);
 }
 
 /* This is called whenever new data may is available */
 static void stream_read_callback(pa_stream *s, size_t length, void *userdata) {
     const void *data;
     assert(s && length);
 
     if (stdio_event)
         mainloop_api->io_enable(stdio_event, PA_IO_EVENT_OUTPUT);
 
     if (pa_stream_peek(s, &data, &length) < 0) {
         fprintf(stderr, "pa_stream_peek() failed: %s\n", pa_strerror(pa_context_errno(context)));
         quit(1);
         return;
     }
 
     assert(data && length);
 
     if (buffer) {
         fprintf(stderr, "Buffer overrun, dropping incoming data\n");
         if (pa_stream_drop(s) < 0) {
             fprintf(stderr, "pa_stream_drop() failed: %s\n", pa_strerror(pa_context_errno(context)));
             quit(1);
         }
         return;
     }
 
     buffer = (float*)pa_xmalloc(buffer_length = length);
     memcpy(buffer, data, length);
     buffer_index = 0;
     pa_stream_drop(s);
 }
 
 /* This routine is called whenever the stream state changes */
 static void stream_state_callback(pa_stream *s, void *userdata) {
     assert(s);
 
     switch (pa_stream_get_state(s)) {
         case PA_STREAM_CREATING:
         case PA_STREAM_TERMINATED:
             break;
 
         case PA_STREAM_READY:
             if (verbose) {
                 const pa_buffer_attr *a;
 
                 fprintf(stderr, "Stream successfully created.\n");
 
                 if (!(a = pa_stream_get_buffer_attr(s)))
                     fprintf(stderr, "pa_stream_get_buffer_attr() failed: %s\n", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
                 else {
 
                     if (mode == PLAYBACK)
                         fprintf(stderr, "Buffer metrics: maxlength=%u, tlength=%u, prebuf=%u, minreq=%u\n", a->maxlength, a->tlength, a->prebuf, a->minreq);
                     else {
                         assert(mode == RECORD);
                         fprintf(stderr, "Buffer metrics: maxlength=%u, fragsize=%u\n", a->maxlength, a->fragsize);
                     }
 
                 }
 
             }
 
             break;
 
         case PA_STREAM_FAILED:
         default:
             fprintf(stderr, "Stream error: %s\n", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
             quit(1);
     }
 }
 
 /* This is called whenever the context status changes */
 static void context_state_callback(pa_context *c, void *userdata) {
     assert(c);
 
     switch (pa_context_get_state(c)) {
         case PA_CONTEXT_CONNECTING:
         case PA_CONTEXT_AUTHORIZING:
         case PA_CONTEXT_SETTING_NAME:
             break;
 
         case PA_CONTEXT_READY: {
             int r;
 
             assert(c && !stream);
 
             if (verbose)
                 fprintf(stderr, "Connection established.\n");
 
             if (!(stream = pa_stream_new(c, stream_name, &sample_spec, channel_map_set ? &channel_map : NULL))) {
                 fprintf(stderr, "pa_stream_new() failed: %s\n", pa_strerror(pa_context_errno(c)));
                 goto fail;
             }
 
             pa_stream_set_state_callback(stream, stream_state_callback, NULL);
             //pa_stream_set_write_callback(stream, stream_write_callback, NULL);
             pa_stream_set_read_callback(stream, stream_read_callback, NULL);
 
		pa_stream_flags_t flags = (pa_stream_flags_t )0;

                 if (((r = pa_stream_connect_record(stream, device, NULL, flags))) < 0) {
                     fprintf(stderr, "pa_stream_connect_record() failed: %s\n", pa_strerror(pa_context_errno(c)));
                     goto fail;
                 }
             
 
             break;
         }
 
         case PA_CONTEXT_TERMINATED:
             quit(0);
             break;
 
         case PA_CONTEXT_FAILED:
         default:
             fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(c)));
             goto fail;
     }
 
     return;
 
 fail:
     quit(1);
 
 }
 
 /* Connection draining complete */
 static void context_drain_complete(pa_context*c, void *userdata) {
     pa_context_disconnect(c);
 }
 
 /* Stream draining complete */
 static void stream_drain_complete(pa_stream*s, int success, void *userdata) {
     pa_operation *o;
 
     if (!success) {
         fprintf(stderr, "Failed to drain stream: %s\n", pa_strerror(pa_context_errno(context)));
         quit(1);
     }
 
     if (verbose)
         fprintf(stderr, "Playback stream drained.\n");
 
     pa_stream_disconnect(stream);
     pa_stream_unref(stream);
     stream = NULL;
 
     if (!(o = pa_context_drain(context, context_drain_complete, NULL)))
         pa_context_disconnect(context);
     else {
         if (verbose)
             fprintf(stderr, "Draining connection to server.\n");
     }
 }
 
 /* New data on STDIN **/
 static void stdin_callback(pa_mainloop_api*a, pa_io_event *e, int fd, pa_io_event_flags_t f, void *userdata) {
     size_t l, w = 0;
     ssize_t r;
     assert(a == mainloop_api && e && stdio_event == e);
 
     if (buffer) {
         mainloop_api->io_enable(stdio_event, PA_IO_EVENT_NULL);
         return;
     }
 
     if (!stream || pa_stream_get_state(stream) != PA_STREAM_READY || !(l = w = pa_stream_writable_size(stream)))
         l = 4096;
 
     buffer = (float*)pa_xmalloc(l);
 
     if ((r = read(fd, buffer, l)) <= 0) {
         if (r == 0) {
             if (verbose)
                 fprintf(stderr, "Got EOF.\n");
 
             if (stream) {
                 pa_operation *o;
 
                 if (!(o = pa_stream_drain(stream, stream_drain_complete, NULL))) {
                     fprintf(stderr, "pa_stream_drain(): %s\n", pa_strerror(pa_context_errno(context)));
                     quit(1);
                     return;
                 }
 
                 pa_operation_unref(o);
             } else
                 quit(0);
 
         } else {
             fprintf(stderr, "read() failed: %s\n", strerror(errno));
             quit(1);
         }
 
         mainloop_api->io_free(stdio_event);
         stdio_event = NULL;
         return;
     }
 
     buffer_length = r;
     buffer_index = 0;
 
     if (w)
         do_stream_write(w);
 }
 
 /* Some data may be written to STDOUT */
 static void stdout_callback(pa_mainloop_api*a, pa_io_event *e, int fd, pa_io_event_flags_t f, void *userdata) {
     ssize_t r;
     assert(a == mainloop_api && e && stdio_event == e);
 
     if (!buffer) {
         mainloop_api->io_enable(stdio_event, PA_IO_EVENT_NULL);
         return;
     }
 
     assert(buffer_length);
 
     if ((r = write(fd, (uint8_t*) buffer+buffer_index, buffer_length)) <= 0) {
         fprintf(stderr, "write() failed: %s\n", strerror(errno));
         quit(1);
 
         mainloop_api->io_free(stdio_event);
         stdio_event = NULL;
         return;
     }
 
     buffer_length -= r;
     buffer_index += r;
 
     if (!buffer_length) {
         pa_xfree(buffer);
         buffer = NULL;
         buffer_length = buffer_index = 0;
     }
 }
 
 /* UNIX signal to quit recieved */
 static void exit_signal_callback(pa_mainloop_api*m, pa_signal_event *e, int sig, void *userdata) {
     if (verbose)
         fprintf(stderr, "Got signal, exiting.\n");
     quit(0);
 }
 
 /* Show the current latency */
 static void stream_update_timing_callback(pa_stream *s, int success, void *userdata) {
     pa_usec_t latency, usec;
     int negative = 0;
 
     assert(s);
 
     if (!success ||
         pa_stream_get_time(s, &usec) < 0 ||
         pa_stream_get_latency(s, &latency, &negative) < 0) {
         fprintf(stderr, "Failed to get latency: %s\n", pa_strerror(pa_context_errno(context)));
         quit(1);
         return;
     }
 
     fprintf(stderr, "Time: %0.3f sec; Latency: %0.0f usec.  \r",
             (float) usec / 1000000,
             (float) latency * (negative?-1:1));
 }
 
 /* Someone requested that the latency is shown */
 static void sigusr1_signal_callback(pa_mainloop_api*m, pa_signal_event *e, int sig, void *userdata) {
 
     if (!stream)
         return;
 
     pa_operation_unref(pa_stream_update_timing_info(stream, stream_update_timing_callback, NULL));
 }
 
 static void time_event_callback(pa_mainloop_api*m, pa_time_event *e, const struct timeval *tv, void *userdata) {
     struct timeval next;
 
     if (stream && pa_stream_get_state(stream) == PA_STREAM_READY) {
         pa_operation *o;
         if (!(o = pa_stream_update_timing_info(stream, stream_update_timing_callback, NULL)))
             fprintf(stderr, "pa_stream_update_timing_info() failed: %s\n", pa_strerror(pa_context_errno(context)));
         else
             pa_operation_unref(o);
     }
 
     pa_gettimeofday(&next);
     pa_timeval_add(&next, TIME_EVENT_USEC);
 
     m->time_restart(e, &next);
 }
 
 int main(int argc, char *argv[]) {
     pa_mainloop* m = NULL;
     int ret = 1, r, c;
     char *bn = argv[0], *server = NULL;
     pa_time_event *time_event = NULL;
     sample_spec.format = PA_SAMPLE_FLOAT32LE;
     sample_spec.rate = 44100;
     sample_spec.channels = 2;
     pa_context_flags_t flags = (pa_context_flags_t)0;

 	verbose = 1;
	
         mode = RECORD;
 
    if (!pa_sample_spec_valid(&sample_spec)) {
         fprintf(stderr, "Invalid sample specification\n");
         goto quit;
     }
 
     if (channel_map_set && channel_map.channels != sample_spec.channels) {
         fprintf(stderr, "Channel map doesn't match sample specification\n");
         goto quit;
     }
 
     if (verbose) {
         char t[PA_SAMPLE_SPEC_SNPRINT_MAX];
         pa_sample_spec_snprint(t, sizeof(t), &sample_spec);
         fprintf(stderr, "Opening a %s stream with sample specification '%s'.\n", mode == RECORD ? "recording" : "playback", t);
     }
 
 
     if (!client_name)
         client_name = pa_xstrdup(bn);
 
	//printf("client name:%s", client_name);
     if (!stream_name)
         stream_name = pa_xstrdup(client_name);
 
     /* Set up a new main loop */
     if (!(m = pa_mainloop_new())) {
         fprintf(stderr, "pa_mainloop_new() failed.\n");
         goto quit;
     }
 
     mainloop_api = pa_mainloop_get_api(m);
 
     r = pa_signal_init(mainloop_api);
     assert(r == 0);
     pa_signal_new(SIGINT, exit_signal_callback, NULL);
     pa_signal_new(SIGTERM, exit_signal_callback, NULL);
 #ifdef SIGUSR1
     pa_signal_new(SIGUSR1, sigusr1_signal_callback, NULL);
 #endif
 #ifdef SIGPIPE
     signal(SIGPIPE, SIG_IGN);
 #endif
 
     if (!(stdio_event = mainloop_api->io_new(mainloop_api,
                                               STDOUT_FILENO,
                                              PA_IO_EVENT_OUTPUT,
                                              stdout_callback, NULL))) {
         fprintf(stderr, "io_new() failed.\n");
         goto quit;
     }
 
     /* Create a new connection context */
     if (!(context = pa_context_new(mainloop_api, client_name))) {
         fprintf(stderr, "pa_context_new() failed.\n");
         goto quit;
     }
 
     pa_context_set_state_callback(context, context_state_callback, NULL);
 

    //device = "alsa_output.pci_8086_284b_alsa_playback_0.monitor";
     /* Connect the context */

     pa_context_connect(context, server, flags, NULL);
 
     if (verbose) {
         struct timeval tv;
 
         pa_gettimeofday(&tv);
         pa_timeval_add(&tv, TIME_EVENT_USEC);
 
         if (!(time_event = mainloop_api->time_new(mainloop_api, &tv, time_event_callback, NULL))) {
             fprintf(stderr, "time_new() failed.\n");
             goto quit;
         }
     }
 
     /* Run the main loop */
     if (pa_mainloop_run(m, &ret) < 0) {
         fprintf(stderr, "pa_mainloop_run() failed.\n");
         goto quit;
     }
 
 quit:
     if (stream)
         pa_stream_unref(stream);
 
     if (context)
         pa_context_unref(context);
 
     if (stdio_event) {
         assert(mainloop_api);
         mainloop_api->io_free(stdio_event);
     }
 
     if (time_event) {
         assert(mainloop_api);
         mainloop_api->time_free(time_event);
     }
 
     if (m) {
         pa_signal_done();
         pa_mainloop_free(m);
     }
 
     pa_xfree(buffer);
 
     pa_xfree(server);
     pa_xfree(device);
     pa_xfree(client_name);
     pa_xfree(stream_name);
 
     return ret;
 }
