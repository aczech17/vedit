#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

typedef struct
{
    int screen_width, screen_height, first_text_line, cursor_x, cursor_y;
}Display_info;

Display_info get_display_info();

#endif // DISPLAY_INFO_H
