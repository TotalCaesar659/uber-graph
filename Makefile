all: uber graph

OBJECTS :=
OBJECTS += uber-graph.o
OBJECTS += uber-list-model.o
OBJECTS += uber-model.o
OBJECTS += uber-pixring.o
OBJECTS += uber-range.o
OBJECTS += uber-renderer.o
OBJECTS += uber-renderer-circle.o

WARNINGS :=
WARNINGS += -Wall
WARNINGS += -Werror

PKGS :=
PKGS += gtk+-3.0

%.o: %.c
	$(CC) -o $@.tmp -c $(WARNINGS) $*.c $(shell pkg-config --cflags $(PKGS))
	mv $@.tmp $@

uber: $(OBJECTS) main.o
	$(CC) -o $@.tmp $(WARNINGS) $(OBJECTS) main.o $(shell pkg-config --libs $(PKGS))
	mv $@.tmp $@

graph: $(OBJECTS) graph.o
	$(CC) -o $@.tmp $(WARNINGS) $(OBJECTS) graph.o $(shell pkg-config --libs $(PKGS))
	mv $@.tmp $@

clean:
	rm -f *.tmp
	rm -f *.o
	rm -f uber
	rm -f graph
