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

#endif // CHARACTER_H
