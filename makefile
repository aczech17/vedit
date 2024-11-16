default:	vedit

vedit:	main.c text.c
	gcc -Wall -Wextra main.c text.c -o vedit
english:	vedit
	./vedit english.txt
empty:	vedit
	./vedit empty.txt
short:	vedit
	./vedit short.txt
macbeth:	vedit
	./vedit macbeth.txt
tadeusz:	vedit
	./vedit pan-tadeusz.txt

