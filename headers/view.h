#ifndef VIEW_H
#define VIEW_H

typedef struct
{
    int screen_width, text_height, log_height, first_text_line, cursor_x, cursor_y;
}View;

View get_view();

#endif // VIEW_H
