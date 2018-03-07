#!/bin/sh
 
search() {
find . \( -name "*.h" -o -name "*.c" -o -name "*.cpp" -o -name "*.cxx" \) -exec grep -Hn "$*" {} \; | grep -v -E '\.\/glx?ew' | grep -v ./Renderer/SOIL2/SOIL2.c | grep -v FBO.cpp
}
 
listIssues() {
search SDL_WM_GrabInput
search SDL_WarpMouse
search SDL_GL
search GL_CLAMP
search glColor|grep -v glColor4f
search glPushAttrib
search glPolygonMode
search GL_QUADS |grep glDrawArrays
search SDL_Init
search SDL_OPENGL
search SDL_ShowCursor
search glClearDepth
search SDL_Quit
search gluBuild2DMipmaps
search glTexImage2D
search glBegin
search glX
search glRect
search glNewList
}
 
listIssues|sort -u
