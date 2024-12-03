#ifndef DISPLAY_H
#define DISPLAY_H

#include "text.h"
#include "view.h"

void display_text(const Text*, const View*);
void clear_line(const View* view, int screen_line);
void display_log(const Text*, const View*);

#endif // DISPLAY_H
