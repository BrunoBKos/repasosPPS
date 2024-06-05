programa: main.o
	gcc -Wall -Wextra -Werror -ansi -pedantic -o programa main.o
main.o: map.o
	gcc -Wall -Wextra -Werror -ansi -pedantic -c main.c -o main.o map.o
map.o:
	gcc -Wall -Wextra -Werror -ansi -pedantic -c map.c -o map.o

clean: 
	rm -f programa
	rm -f *.o