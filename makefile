default:	vedit

vedit:	src/main.c src/text.c
	gcc -Wall -Wextra src/main.c src/text.c -o vedit
english:	vedit
	./vedit test/english.txt
empty:	vedit
	./vedit test/empty.txt
short:	vedit
	./vedit test/short.txt
macbeth:	vedit
	./vedit test/macbeth.txt
tadeusz:	vedit
	./vedit test/pan-tadeusz.txt

