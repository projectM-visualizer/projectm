These tests are currently built only if `./configure --enable-sdl` is run.  The one working
test doesn't actually require SDL, but the others are GUI.  

* `projectM-unittest` simply runs TestRunner::run() and returns 0 (success) or 1 (failure)

The other tests need to be updated to SDL2.
