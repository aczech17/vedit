#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "text.h"
#include "display_info.h"
#include "display.h"
#include "input.h"

void console_setup()
{
    SetConsoleOutputCP(CP_UTF8);
    DWORD prev_mode;
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hInput, &prev_mode);
    SetConsoleMode(hInput, prev_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
}

void update_view(const Text* text, Display_info* display_info)
{
    if (display_info->cursor_y >= text->line_count)
    {
        display_info->cursor_y = text->line_count - 1;
        return;
    }


    if (display_info->cursor_y == display_info->screen_height)
    {
        ++display_info->first_text_line;
        display_info->cursor_y = display_info->screen_height - 1;
    }


    if (display_info->cursor_y < 0)
    {
        if (display_info->first_text_line > 0)
            --display_info->first_text_line;
        display_info->cursor_y = 0;
    }
}

int main(int argc, char** argv)
{
    // 30 LINII

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
        int input_key = read_input(text, &display_info);
        if (input_key == VK_ESCAPE)
            break;

        // if (display_info.cursor_y >= text->line_count)
        // {
        //     exit(text->line_count);
        // }
        update_view(text, &display_info);
        display(text, &display_info);
    }

    // free_text(text);
    system("cls");

    printf("ostatnia linia: %s\n", text->lines[text->line_count - 1]);
    return 0;
}
