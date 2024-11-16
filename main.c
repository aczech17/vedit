#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "text_info.h"

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

void print_file_content(const char* content, const Screen_info* screen_info, int cursor_x, int text_line)
{
    int line_count = screen_info->height;
    int screen_width = screen_info->width;

    // skip first text_line lines
    for (int i = 0; i < text_line; ++i)
    {
        const char* next_line = strstr(content, "\n");
        if (next_line == NULL)
            return;
        
        content = next_line + 1;
    }

    for (int screen_line = 0; screen_line < line_count; ++screen_line)
    {
        char* next_line = strstr(content, "\n");
        if (next_line == NULL)
        {
            int line_size = strlen(content);
            print_line(content, line_size, screen_width, cursor_x, screen_line);
            break;
        }

        int line_size = next_line - content;
        print_line(content, line_size, screen_width, cursor_x, screen_line);

        content = next_line + 1;
    }
}

char* get_file_content(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    size_t file_size = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(file_size + 1);
    size_t bytes_read = fread(content, 1, file_size, file);
    if (bytes_read < file_size)
    {
        free(content);
        fclose(file);
        return NULL;
    }

    content[file_size] = 0;
    fclose(file);

    return content;
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
        fprintf(stderr, "Nazwa pliku.\n");
        return 1;
    }

    char* filename = argv[1];
    char* content = get_file_content(filename);
    Text_info* text_info = get_text_info(content);
    int line_count = text_info != NULL ? text_info->line_count : 0;

    int pos_x = 0, pos_y = 0;
    int first_text_line = 0;
    INPUT_RECORD input_record;
    DWORD events;

    system("cls");
    for (;;)
    {
        int text_line = first_text_line + pos_y;
        int line_size = text_info->line_sizes[text_line];
        if (pos_x > line_size)
            pos_x = line_size;

        print_file_content(content, &screen_info, pos_x, first_text_line);
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
    free_text_info(text_info);
    free(content);

    system("cls");
    return 0;
}
