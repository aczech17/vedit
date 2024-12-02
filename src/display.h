#ifndef DISPLAY_H
#define DISPLAY_H

#include "text.h"
#include "display_info.h"

void display_text(const Text*, const Display_info*);
void clear_line(const Display_info* display_info, int screen_line);
void display_log(const Text*, const Display_info*);
#endif // DISPLAY_H
