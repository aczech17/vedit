#ifdef _WIN32
#include <windows.h>
#elif __linux__
#else
#error "Unsupported OS."
#endif

#include <stdio.h>
#include <stdlib.h>
#include "text.h"
#include "display_info.h"
#include "display.h"
#include "input.h"

void console_setup()
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        DWORD prev_mode;
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hInput, &prev_mode);
        SetConsoleMode(hInput, prev_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    #elif __linux__
        system("stty raw -echo");
    #else
        #error "Terminal configuration for this OS is not implemented."
    #endif
}

void console_cleanup()
{
    #ifdef _WIN32
        system("cls");
    #elif __linux__
        system("stty cooked echo"); // Reset to default settings.
        system("clear");
    #endif
}

void update_text(Text* text, Display_info* display_info, int input_key)
{
    switch (input_key)
    {
        case VK_RETURN:
        {
            int current_text_line = display_info->first_text_line + display_info->cursor_y;
            int line_split_position = display_info->cursor_x;

            split_lines(text, current_text_line, line_split_position);
            display_info->cursor_y++;
            break;
        }
    }
}

void update_view(const Text* text, Display_info* display_info, int input_key)
{
    int current_text_line = display_info->first_text_line + display_info->cursor_y;
    int current_text_line_size = strlen(text->lines[current_text_line]);

    switch (input_key)
    {
        case VK_DOWN:
        {
            display_info->cursor_y++;
            break;
        }
        case VK_UP:
        {
            display_info->cursor_y--;
            break;
        }
        case VK_LEFT:
        {
            display_info->cursor_x--;
            break;
        }
        case VK_RIGHT:
        {
            display_info->cursor_x++;
            break;
        }
        case VK_HOME:
        {
            display_info->cursor_x = 0;
            break;
        }
        case VK_END:
        {
            display_info->cursor_x = current_text_line_size;
            break;
        }
    }

    if (display_info->cursor_x >= current_text_line_size)
        display_info->cursor_x = current_text_line_size;

    if (display_info->cursor_y == text->line_count)
        display_info->cursor_y = text->line_count - 1;

    if (display_info->cursor_y == display_info->text_height)
    {
        int new_last_line = display_info->first_text_line + display_info->text_height - 1;
        if (new_last_line < text->line_count - 1)
            display_info->first_text_line++;
        display_info->cursor_y--;
    }

    if (display_info->cursor_y < 0)
    {
        if (display_info->first_text_line > 0)
            display_info->first_text_line--;

        display_info->cursor_y = 0;
    }
}

int main(int argc, char** argv)
{
    console_setup();   
    Display_info display_info = get_display_info();
    
    if (argc < 2)
    {
        fprintf(stderr, "Filename missing.\n");
        return 1;
    }

    char* filename = argv[1];
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open the file.\n");
        return 2;
    }

    Text* text = get_text(file);
    fclose(file);

    if (text == NULL)
    {
        fprintf(stderr, "Could not parse the file.\n");
        return 3;
    }
 
    system("cls");
    for (;;)
    {
        int input_key = read_input();
        if (input_key == VK_ESCAPE)
            break;

        update_text(text, &display_info, input_key);
        update_view(text, &display_info, input_key);
        display_text(text, &display_info);
        display_log(text, &display_info);
    }

    free_text(text);

    console_cleanup();
    return 0;
}
