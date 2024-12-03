SRC = src/main.c src/update.c src/text.c src/display.c src/view.c src/input.c src/console_utils.c

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

