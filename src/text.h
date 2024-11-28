#ifndef TEXT_H
#define TEXT_H

#include <stdio.h>

typedef struct
{
    char** lines;
    int line_count;
    int capacity;
}Text;

Text* get_text(FILE*);
void push_character(Text* text, int line_number, int char_position, char value);
void delete_character(Text* text, int line_number, int char_position);
void delete_line(Text* text, int line_number);
void free_text(Text*);

#endif // TEXT_H
