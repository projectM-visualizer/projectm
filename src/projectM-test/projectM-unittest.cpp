/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2019-2019 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include <projectM.hpp>
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include "../libprojectM/TestRunner.hpp"
#include <SDL2/SDL.h>

int main(int argc, char **argv)
{
    SDL_Window *win = SDL_CreateWindow("projectM", 0, 0, 100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glCtx = SDL_GL_CreateContext(win);

    // try unit tests
    if (!TestRunner::run())
    {
        printf("unit tests failed\n");
        exit(1);
    }
    printf("unit tests succeeded\n");

    SDL_GL_DeleteContext(glCtx);
    exit(0);
}
