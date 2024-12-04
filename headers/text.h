#ifndef TEXT_H
#define TEXT_H

#include <stdio.h>

typedef struct
{
    char** lines;
    int line_count;
    int capacity;
}Text;

Text* empty_text();
Text* get_text_from_file(FILE*);
void push_character(Text* text, int line_number, int char_position, char value);
void delete_character(Text* text, int line_number, int char_position);
void delete_line(Text* text, int line_number);
void split_lines(Text* text, int line_number, int split_position);
void join_lines(Text* text, int upper_line_number);
void deallocate_text(Text*);

#endif // TEXT_H
