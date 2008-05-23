

#include <libvisual/libvisual.h> 
 
#include <stdlib.h> 
 
#include "display.h" 
#include "sdldriver.h" 
 
 
int main (int argc, char **argv) 
{ 
SADisplay *display; 
VisVideo *video; 
 
VisBin *bin; 
 
VisEventQueue *localqueue; 
VisVideoAttributeOptions *vidoptions; 
 
//char *actorn = "gforce"; 
/* Setting projectm to default visualization  
* Make sure you have it correctly installed ... */ 
char *actorn = "projectm"; 
char *inputn = "alsa"; 
 
int running = TRUE; 
int fullscreen = FALSE;
int visible = TRUE; 
 
int depth; 
 
visual_init (&argc, &argv); 
 
display = display_new (sdl_driver_new ()); 
 
/* Libvisual stuff */ 
if (argc > 1) 
actorn = argv[1]; 
 
if (argc > 2) 
inputn = argv[2]; 
 
bin = visual_bin_new (); 
visual_bin_set_supported_depth (bin, VISUAL_VIDEO_DEPTH_ALL); 
 
visual_bin_connect_by_names (bin, actorn, inputn); 
 
vidoptions = visual_actor_get_video_attribute_options (visual_bin_get_actor (bin)); 
 
depth = visual_video_depth_get_highest (visual_actor_get_supported_depth (visual_bin_get_actor (bin))); 
 
display_create (display, depth, vidoptions, 1024, 768, TRUE); 
 
video = display_get_video (display); 
visual_bin_set_video (bin, video); 
 
visual_bin_depth_changed (bin); 
 
visual_bin_switch_set_style (bin, VISUAL_SWITCH_STYLE_MORPH); 
visual_bin_switch_set_automatic (bin, TRUE); 
visual_bin_switch_set_mode (bin, VISUAL_MORPH_MODE_TIME); 
visual_bin_switch_set_time (bin, 4, 0); 
 
visual_bin_realize (bin); 
 
visual_bin_sync (bin, FALSE); 
 
 
localqueue = visual_event_queue_new (); 
/* RUNNING IN FULLSCREEN BY DEFAULT */ 
display_set_fullscreen (display, fullscreen, FALSE); 
 
while (running) { 
VisEventQueue *pluginqueue; 
VisEvent *ev; 
 
/* Handle all events */ 
display_drain_events (display, localqueue); 
 
pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin (visual_bin_get_actor (bin))); 
while (visual_event_queue_poll_by_reference (localqueue, &ev)) { 
 
if (ev->type != VISUAL_EVENT_RESIZE) 
visual_event_queue_add (pluginqueue, ev); 
 
switch (ev->type) { 
case VISUAL_EVENT_RESIZE: 
video = display_get_video (display); 
visual_bin_set_video (bin, video); 
 
visual_bin_sync (bin, FALSE); 
break; 
 
case VISUAL_EVENT_MOUSEMOTION: 
break; 
 
case VISUAL_EVENT_MOUSEBUTTONDOWN: 
case VISUAL_EVENT_MOUSEBUTTONUP: 
break; 
 
case VISUAL_EVENT_KEYDOWN: 
switch (ev->event.keyboard.keysym.sym) { 
case VKEY_ESCAPE: 
running = FALSE; 
break; 
case VKEY_TAB: 
fullscreen = !fullscreen; 
 
display_set_fullscreen (display, fullscreen, TRUE); 
 
/* Resync video */ 
video = display_get_video (display); 
visual_bin_set_video (bin, video); 
 
visual_bin_sync (bin, FALSE); 
 
break; 
 
case VKEY_PAGEUP: 
visual_bin_set_morph_by_name (bin, "tentacle"); 
 
visual_bin_switch_actor_by_name (bin, "oinksie"); 
 
break; 
 
case VKEY_PAGEDOWN: 
 
break; 
 
default: 
break; 
} 
 
break; 
 
case VISUAL_EVENT_KEYUP: 
break; 
 
case VISUAL_EVENT_QUIT: 
running = FALSE; 
break; 
 
case VISUAL_EVENT_VISIBILITY: 
visible = ev->event.visibility.is_visible; 
break; 
 
default: 
break; 
} if (visual_bin_depth_changed (bin) == TRUE) { 
vidoptions = visual_actor_get_video_attribute_options (visual_bin_get_actor (bin)); 
depth = visual_bin_get_depth (bin); 
 
display_create (display, depth, vidoptions, video->width, video->height, TRUE); 
 
video = display_get_video (display); 
 
visual_bin_set_video (bin, video); 
 
visual_bin_sync (bin, TRUE); 
} 
 
/* Do a run cycle */ 
display_lock (display); 
display_unlock (display); 
 
visual_bin_run (bin); 
 
display_update_all (display); 
 
display_fps_limit (display, 30); 
} 
 
/* Termination procedure */ 
display_set_fullscreen (display, FALSE, TRUE); 
 
printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display)); 
 
return 0; 
} 
}