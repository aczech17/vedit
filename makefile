SRC = code/src/main.c code/src/character.c code/src/update.c code/src/text.c code/src/display.c code/src/view.c code/src/input.c code/src/console_utils.c

default:	vedit

vedit:	$(SRC)
	gcc -Wall -Wextra -O3 $(SRC) -o vedit
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

