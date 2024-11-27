#include "display_info.h"
#include <windows.h>

Display_info get_display_info()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int screen_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int screen_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    Display_info info = 
    {
        .screen_width = screen_width,
        .screen_height = screen_height,
        .first_text_line = 0,
        .cursor_x = 0,
        .cursor_y = 0,
    };
    
    return info;
}
