run:	vedit
	./vedit makefile
vedit:	main.c
	gcc -Wall -Wextra main.c -o vedit
