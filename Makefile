project: proj.c
	gcc -Wall -Wextra -pedantic -ansi -O3 proj.c -o project

debug: proj.c
	gcc -Wall -Wextra -pedantic -ansi -O3 -g proj.c -o project
clean:
	rm project