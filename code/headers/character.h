#ifndef CHARACTER_H
#define CHARACTER_H

typedef enum
{
    UP, DOWN, LEFT, RIGHT,
    HOME, END,
    ENTER, BACKSPACE,
    ESCAPE,
    F1,
    
    ALPHANUMERIC,
    NONE
}Character_type;

typedef struct
{
    Character_type character_type;
    char bytes[4];
    int size;
}Character;

int length_of_characters(char* string, int character_count);
int character_count_of_string(char* string);

#endif // CHARACTER_H
