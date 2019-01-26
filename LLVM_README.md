### Building with LLVM JIT

Install the LLVM libraries.  I've used llvm-7 so far, see http://apt.llvm.org/ for LLVM repositories.

```
sudo apt-get install llvm-7-dev llvm-7-tools
```

Then run these commands to configure

```
./autogen.sh && ./configure --enable-sdl --enable-qt --enable-llvm --prefix=$PWD/local
```

And a clean build of course.

```
make clean && make -j4 install
```
