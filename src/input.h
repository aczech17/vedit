#ifndef INPUT_H
#define INPUT_H

#include "text.h"
#include "display_info.h"

typedef enum
{
    UP, DOWN, LEFT, RIGHT,
    HOME, END,
    ENTER, BACKSPACE,
    ESCAPE,
    ALPHANUMERIC,
    NONE
}Key_type;

typedef struct
{
    Key_type key_type;
    int value;
}Key_code;

Key_code read_input();

#endif // INPUT_H
