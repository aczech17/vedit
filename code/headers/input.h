#ifndef INPUT_H
#define INPUT_H

#include "text.h"

typedef enum
{
    UP, DOWN, LEFT, RIGHT,
    HOME, END,
    ENTER, BACKSPACE,
    ESCAPE,
    F1,
    
    ALPHANUMERIC,
    NONE
}Key_type;

typedef struct
{
    Key_type key_type;
    int value;
}Key_code;

typedef enum
{
    WATCH, EDIT, SAVE,
}Mode;

Key_code read_input();
char* mode_to_str(const Mode mode);

#endif // INPUT_H
