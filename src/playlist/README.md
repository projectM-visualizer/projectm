ProjectM Playlist Library
=========================

This is an optional convenience library to load presets into a playlist,
manage their order and chose the next one to play.

The core library uses a callback to request the next preset filename when a switch should occur and has no idea about a
playlist at all. Application developers not wanting to go through the hurdles of implementing their own playlist
management can use this library to perform this task.