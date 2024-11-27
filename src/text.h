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
void free_text(Text*);

#endif // TEXT_H
