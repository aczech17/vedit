default:	vedit

empty:	vedit
	./vedit empty.txt

short:	vedit
	./vedit short.txt
run:	vedit
	./vedit macbeth.txt
vedit:	main.c
	gcc -Wall -Wextra main.c -o vedit
