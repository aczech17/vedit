#ifndef INPUT_H
#define INPUT_H

#include "text.h"
#include "character.h"

typedef enum
{
    WATCH, EDIT, READ_PATH, SAVE, QUIT,
}Mode;

Character read_input();
char* mode_to_str(const Mode mode);

#endif // INPUT_H
