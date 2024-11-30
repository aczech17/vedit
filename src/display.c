#include "display.h"
#include "display_info.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <stdio.h>
#endif

static void set_cursor_position(int x, int y)
{
    #ifdef _WIN32
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){x, y});
    #elif __linux__
        printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape sequence is 1-indexed.
        fflush(stdout);
    #else
        #error "Cursor manipulation in this OS is not implemented."
    #endif
}

static void write_line_to_console(char* line, int line_size, int screen_line)
{
    #ifdef _WIN32
        static DWORD written;
        WriteConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), line, line_size, (COORD){0, screen_line}, &written);
    #elif __linux__
        printf("\033[%d;1H", screen_line + 1); // // ANSI escape sequence is 1-indexed.
        fwrite(line, 1, line_size, stdout);
        fflush(stdout);
    #else
        #error "Writing to console for this OS is not implemented."
    #endif
}

static void print_line(const char* line, int line_size, int screen_width, int cursor_x, int screen_line)
{
    char* buffer = malloc(screen_width + 1);
    memset(buffer, ' ', screen_width);
    buffer[screen_width] = 0;

    int start_index = (cursor_x / screen_width) * screen_width;
    int end_index = start_index + screen_width;
    if (end_index >= line_size)
        end_index = line_size;

    if (start_index < line_size)
        strncpy(buffer, line + start_index, end_index - start_index);        
    
    write_line_to_console(buffer, screen_width, screen_line);
    free(buffer);
}

void display_text(const Text* text, const Display_info* info)
{
    int line_count = text->line_count < info->text_height ? text->line_count : info->text_height;
    int screen_width = info->screen_width;

    for (int screen_line = 0; screen_line < line_count; ++screen_line)
    {
        int text_line_number = screen_line + info->first_text_line;
        char* line = text->lines[text_line_number];
        int line_size = strlen(text->lines[text_line_number]);

        print_line(line, line_size, screen_width, info->cursor_x, screen_line);
    }

    set_cursor_position(info->cursor_x % info->screen_width, info->cursor_y);
}

void display_log(const Text* text, const Display_info* display_info)
{
    char* log = malloc(display_info->screen_width + 1);
    memset(log, ' ', display_info->screen_width);
    log[display_info->screen_width] = 0;

    int selected_text_line = display_info->first_text_line + display_info->cursor_y;
    sprintf(log, "%d, %d \t %d lines", display_info->cursor_x, selected_text_line, text->line_count);
    write_line_to_console(log, display_info->screen_width, display_info->text_height);

    memset(log, ' ', display_info->screen_width);
    sprintf(log, "cursor: %d, %d", display_info->cursor_x, display_info->cursor_y);
    write_line_to_console(log, display_info->screen_width, display_info->text_height + 1);

    free(log);
}
