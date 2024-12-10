#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#else
#error "Unsupported OS."
#endif

#include "../headers/console_utils.h"
#include <stdlib.h>

void console_setup()
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        DWORD prev_mode;
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hInput, &prev_mode);
        SetConsoleMode(hInput, prev_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    #elif __linux__
        struct termios old_tio, new_tio;

        tcgetattr(STDIN_FILENO, &old_tio);

        new_tio = old_tio;
        new_tio.c_lflag &= ~(ICANON | ECHO);        // Disable buffering and echo.
        new_tio.c_cc[VMIN] = 1;                     // Waiting for 1 character (non-blocking).
        new_tio.c_cc[VTIME] = 0;                    // Set time for waiting for a character.

        tcsetattr(STDIN_FILENO, TCSANOW, &new_tio); // Apply new settings.
    #else
        #error "Terminal configuration for this OS is not implemented."
    #endif
}

void console_cleanup()
{
    #ifdef _WIN32
        system("cls");
    #elif __linux__
        // Restore old terminal settings.
        struct termios old_tio;
        tcgetattr(STDIN_FILENO, &old_tio);

        old_tio.c_lflag |= (ICANON | ECHO);   // Restore buffering and echo.
        tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

        printf("\033[?25h"); // Set the cursor on.
        system("clear");
    #endif
}

void clear_screen()
{
    #ifdef _WIN32
        system("cls");
    #elif __linux__
        system("clear");
    #endif
}
