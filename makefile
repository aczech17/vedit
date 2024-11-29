SRC = src/display_info.c src/display.c src/input.c src/main.c src/text.c

default:	vedit

vedit:	$(SRC)
	gcc -Wall -Wextra $(SRC) -o vedit
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

