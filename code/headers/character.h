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

int length_of_characters(const char* string, int character_count);
int count_of_characters(const char* string);
char* get_characters_range(const char* string, int lower, int upper);

#endif // CHARACTER_H
