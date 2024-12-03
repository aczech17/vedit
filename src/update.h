#ifndef UPDATE_H
#define UPDATE_H

#include "text.h"
#include "view.h"
#include "input.h"

void update_text(Text* text, View* view, Key_code input_key);
void update_view(const Text* text, View* view, Key_code input_key);

#endif // UPDATE_H
