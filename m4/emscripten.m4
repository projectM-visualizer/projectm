AC_DEFUN([AM_EMSCRIPTEN],
[
  AC_MSG_CHECKING(for emscripten compiler)
  emscripten_compiler=no
  AC_TRY_COMPILE([
      #ifndef EMSCRIPTEN
      #error "not an emscripten compiler"
      #endif
      ],[
      ],[
       host=javascript-web-emscripten
       emscripten_compiler=yes
       cross_compiling=yes
      ])
  AC_MSG_RESULT($emscripten_compiler)
  AM_CONDITIONAL(EMSCRIPTEN, [test "x$emscripten_compiler" = "xyes"])
])
