default:	vedit

vedit:	main.c text_info.c
	gcc -Wall -Wextra main.c text_info.c -o vedit

empty:	vedit
	./vedit empty.txt
short:	vedit
	./vedit short.txt
macbeth:	vedit
	./vedit macbeth.txt
tadeusz:	vedit
	./vedit pan-tadeusz.txt

