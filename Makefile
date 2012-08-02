all: uber-graph

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
	main.o								\
	blktrace.o							\
	sysmon.o							\
	g-ring.o							\
	$(NULL)



ifeq ($(DISABLE_DEBUG),1)
	INCLUDES += $(DEBUG_INCLUDES)
endif

main.o: main.c Makefile
	$(CC) -c -o $@ -g $(WARNINGS) $(INCLUDES) main.c $(shell pkg-config --cflags gtk+-3.0 gthread-2.0)

%.o: %.c %.h Makefile
	$(CC) -c -o $@ -g $(WARNINGS) $(INCLUDES) $*.c $(shell pkg-config --cflags gtk+-3.0 gthread-2.0)

uber-graph: $(OBJECTS) Makefile
	$(CC) -o $@ -g $(OBJECTS) $(shell pkg-config --libs gtk+-3.0 gthread-2.0)

clean:
	rm -f uber-graph $(OBJECTS)

run: uber-graph
	./uber-graph
