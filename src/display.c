#include "display.h"
#include "display_info.h"
#include <windows.h>

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
    
    DWORD written;
    WriteConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, screen_width, (COORD){0, screen_line}, &written);
    free(buffer);
}

void display(const Text* text, const Display_info* info)
{
    int line_count = text->line_count < info->screen_height ? text->line_count : info->screen_height;
    int screen_width = info->screen_width;

    for (int screen_line = 0; screen_line < line_count; ++screen_line)
    {
        int text_line_number = screen_line + info->first_text_line;
        char* line = text->lines[text_line_number];
        int line_size = strlen(text->lines[text_line_number]);

        print_line(line, line_size, screen_width, info->cursor_x, screen_line);
    }

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){info->cursor_x % info->screen_width, info->cursor_y});
}
