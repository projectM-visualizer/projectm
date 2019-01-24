### Building with LLVM JIT

I haven't updated ./autogen.sh && ./configure yet.  However, you can build with LLVM JIT enabled by getting the LLVM development libraries

```
sudo apt-get install llvm-7-dev llvm-7-tools
```

Then run these commands to configure

```
export LDFLAGS="$(llvm-config --ldflags)"
export LIBS="$(llvm-config --libs)"
export CXXFLAGS='-DNDEBUG -O2 -g'
./autogen.sh && ./configure --enable-sdl --enable-qt --prefix=$PWD/local
```

And a clean build of course.

```make clean && make -j4 install```
