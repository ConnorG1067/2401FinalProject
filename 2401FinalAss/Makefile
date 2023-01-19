EXECS = defs.h main.c room.c ghost.c hunter.c building.c
LIBS = -lpthread

all:	$(EXECS)

fa:  $(EXECS)
	gcc -o fa $(EXECS) $(LIBS) -g

main: main.c
	gcc -o main main.c $(LIBS)

clean:
	rm -f $(EXECS)