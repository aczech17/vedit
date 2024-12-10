#ifndef TEXT_H
#define TEXT_H

#include <stdbool.h>
#include <stdio.h>
#include "character.h"

typedef struct
{
    char** lines;
    int line_count;
    int capacity;
    bool modified;
}Text;

Text* empty_text();
Text* get_text_from_file(FILE*);
void push_character(Text* text, int line_number, int char_position, Character character);
void delete_character(Text* text, int line_number, int char_position);
void delete_line(Text* text, int line_number);
void split_lines(Text* text, int line_number, int split_position);
void join_lines(Text* text, int upper_line_number);
bool save_text(const Text* text, const char* filename);
void deallocate_text(Text*);

#endif // TEXT_H
