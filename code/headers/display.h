#ifndef DISPLAY_H
#define DISPLAY_H

#include "text.h"
#include "view.h"
#include "input.h"

void set_cursor_position(int x, int y);
void display_text(const Text*, const View*);
void clear_line(const View* view, int screen_line);
void display_log(const Text*, const View*, const Mode mode);

#endif // DISPLAY_H
