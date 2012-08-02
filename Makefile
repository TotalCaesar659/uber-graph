all: system-monitor

DISABLE_DEBUG := 0

WARNINGS =								\
	-Wall								\
	-Werror								\
	-Wold-style-definition						\
	-Wdeclaration-after-statement					\
	-Wredundant-decls						\
	-Wmissing-noreturn						\
	-Wcast-align							\
	-Wwrite-strings							\
	-Winline							\
	-Wformat-nonliteral						\
	-Wformat-security						\
	-Wswitch-enum							\
	-Wswitch-default						\
	-Winit-self							\
	-Wmissing-include-dirs						\
	-Wundef								\
	-Waggregate-return						\
	-Wmissing-format-attribute					\
	-Wnested-externs

#	-Wshadow

DEBUG_INCLUDES =							\
	-DG_DISABLE_ASSERT						\
	-DG_DISABLE_CHECKS						\
	-DG_DISABLE_CAST_CHECKS						\
	-DDISABLE_DEBUG							\
	$(NULL)

INCLUDES =								\
	-I../								\
	-DGTK_DISABLE_SINGLE_INCLUDES					\
	-DGDK_DISABLE_DEPRECATED					\
	-DGTK_DISABLE_DEPRECATED					\
	$(NULL)

OBJECTS =								\
	uber-graph.o							\
	uber-line-graph.o						\
	uber-window.o							\
	uber-scale.o							\
	uber-label.o							\
	uber-range.o							\
	uber-frame-source.o						\
	uber-timeout-interval.o						\
	uber-heat-map.o						\
	uber-scatter.o							\
	blktrace.o							\
	sysmon.o							\
	g-ring.o							\
	$(NULL)



ifeq ($(DISABLE_DEBUG),1)
	INCLUDES += $(DEBUG_INCLUDES)
endif

system-monitor.o: system-monitor.c Makefile
	$(CC) -c -o $@ -g $(WARNINGS) $(INCLUDES) system-monitor.c $(shell pkg-config --cflags gtk+-3.0 gthread-2.0)

uber-demo.o: uber-demo.c Makefile
	$(CC) -c -o $@ -g $(WARNINGS) $(INCLUDES) uber-demo.c $(shell pkg-config --cflags gtk+-3.0 gthread-2.0)

%.o: %.c %.h Makefile
	$(CC) -c -o $@ -g $(WARNINGS) $(INCLUDES) $*.c $(shell pkg-config --cflags gtk+-3.0 gthread-2.0)

system-monitor: system-monitor.o $(OBJECTS) Makefile
	$(CC) -o $@ -g system-monitor.o $(OBJECTS) $(shell pkg-config --libs gtk+-3.0 gthread-2.0)

uber-demo: uber-demo.o $(OBJECTS) Makefile
	$(CC) -o $@ -g uber-demo.o $(OBJECTS) $(shell pkg-config --libs gtk+-3.0 gthread-2.0)


clean:
	rm -f system-monitor $(OBJECTS)

run: system-monitor
	./system-monitor
