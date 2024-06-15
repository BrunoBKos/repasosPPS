SOURCES=map.c
OBJECTS=$(SOURCES:.c=.o)

libmap.a: $(OBJECTS)
	ar rcs libmap.a $(OBJECTS)

$(OBJECTS): $(SOURCES)
	gcc -c $(SOURCES) -ansi -pedantic -Wall -Wextra

test: test.c
	gcc -o test test.c -ansi -pedantic -Wall -Wextra -lmap -L. -g

clean:
	rm -f test libmap.a
	rm -f *.o