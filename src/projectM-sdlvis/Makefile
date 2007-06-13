#
#
#

SUBDIRS=
RANLIB=ranlib
INCLUDES=-I. -I.. -I../projectm -I/usr/include/xmms -I/usr/include/glib-1.2 -I/usr/lib/glib/include -DHAVE_OPENGL -I/usr/include/glib-1.2 -I/usr/lib/glib/include -I/usr/include/SDL
DEFINES= -DLINUX -DHAVE_OPENGL -DHAVE_CONFIG_H -D_THREAD_SAFE
CFLAGS=-O2 
#CFLAGS=-g
LIBRARIES= -L/usr/lib -lglib -L/usr/lib -lSDL -lGL -lGLU

LD_FLAGS=-lprojectM -L../projectM -L/usr/local/lib $(LIBRARIES)

SRCS=\
projectMvis.cc
OBJS=$(SRCS:.c=.o)

.SUFFIXES: .c

all: projectMvis

### Relinks the executable
projectMvis: $(OBJS)
	rm -f projectMvis
	$(CC) -o projectMvis $(OBJS) $(LD_FLAGS)


$(LIB_TOP):
	@( if [ ! -d $(LIB_TOP) ]; then \
	( mkdir $(LIB_TOP) ) \
	fi )

$(OBJS): %.o: %.c
	$(CC) -c $(INCLUDES) $(CFLAGS) $(DEFINES) $*.c -o $*.o

clean:
	rm -f *.o
	rm -f projectMvis

distcln: cln
	rm -f config.log
	rm -f config.cache
	rm -f config.status

install: 
	mkdir -p /etc/projectM/fonts
	mkdir -p /etc/projectM/presets
	cp projectMvis /usr/local/bin/projectMvis
	cp -r ../../fonts /etc/projectM
	cp -r ../../presets_projectM /etc/projectM
	chmod -R 755 /etc/projectM
	chmod -R 777 /etc/projectM/presets

