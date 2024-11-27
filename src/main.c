#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "text.h"

#define STD GetStdHandle(STD_OUTPUT_HANDLE)

typedef struct
{
    int width;
    int height;
}Screen_info;

Screen_info get_screen_info()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(STD, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    Screen_info info = {.width = width, .height = height};
    return info;
}

void print_line(const char* line, int line_size, int screen_width, int cursor_x, int screen_line)
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
    WriteConsoleOutputCharacterA(STD, buffer, screen_width, (COORD){0, screen_line}, &written);
    free(buffer);
}

void print_file_content(const Text* text, const Screen_info* screen_info, int cursor_x, int first_text_line)
{
    int line_count = text->line_count < screen_info->height ? text->line_count : screen_info->height;
    int screen_width = screen_info->width;

    for (int screen_line = 0; screen_line < line_count; ++screen_line)
    {
        int text_line_number = screen_line + first_text_line;
        char* line = text->lines[text_line_number];
        int line_size = text->line_sizes[text_line_number];

        print_line(line, line_size, screen_width, cursor_x, screen_line);
    }
}


int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    DWORD prev_mode;
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hInput, &prev_mode);
    SetConsoleMode(hInput, prev_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));


    Screen_info screen_info = get_screen_info();
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

    int line_count = text != NULL ? text->line_count : 0;

    int pos_x = 0, pos_y = 0;
    int first_text_line = 0;
    INPUT_RECORD input_record;
    DWORD events;

    system("cls");
    for (;;)
    {
        int text_line = first_text_line + pos_y;
        int line_size = text->line_sizes[text_line];
        if (pos_x > line_size)
            pos_x = line_size;

        print_file_content(text, &screen_info, pos_x, first_text_line);
        SetConsoleCursorPosition(STD, (COORD){pos_x % screen_info.width, pos_y});

        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events);
        if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) 
        {
            switch (input_record.Event.KeyEvent.wVirtualKeyCode) 
            {
                case VK_UP:
                {
                    if (pos_y > 0)
                        --pos_y;
                    else if (first_text_line > 0)
                        --first_text_line;

                    break;
                }
                case VK_DOWN:
                {
                    if (pos_y >= line_count - 1)
                        break;
                    if (pos_y < screen_info.height - 1)
                        ++pos_y;
                    else
                    {
                        if (first_text_line < line_count - 1)
                            ++first_text_line;
                    }
                        
                    break;
                }
                case VK_LEFT:
                    if (pos_x > 0) 
                    {
                        pos_x--;
                    }
                    break;
                case VK_RIGHT:
                {
                    if (pos_x == line_size)
                        break;
                    pos_x++;
                    break;
                }
                case VK_HOME:
                    pos_x = 0;
                    break;
                case VK_END:
                    pos_x = line_size;
                    break;
                case VK_ESCAPE:
                    goto end;
            }
        }
    }

end:
    free_text(text);
    // free(content);

    system("cls");
    return 0;
}
