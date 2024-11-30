#include "display_info.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/ioctl.h>
#endif

Display_info get_display_info()
{
    int screen_width, screen_height;

    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        screen_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        screen_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #elif __linux__
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
        {
            screen_width = ws.ws_col;
            screen_height = ws.ws_row;
        }
        else
        {
            perror("ioctl TIOCGWINSZ failed. Set to default 80 x 24.");
            screen_width = 80;
            screen_height = 24;
        }
    #else
        #error "Gathering terminal info for this OS is not implemented."
    #endif


    const int log_height = 2;

    Display_info info = 
    {
        .screen_width = screen_width,
        .text_height = screen_height - log_height,
        .log_height = log_height,
        .first_text_line = 0,
        .cursor_x = 0,
        .cursor_y = 0,
    };
    
    return info;
}
