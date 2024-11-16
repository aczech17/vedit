#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

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

void print_line(const char* line, int line_size, int screen_width, int pos_x, int screen_line)
{
    int start_index = (pos_x / screen_width) * screen_width;
    int end_index = start_index + screen_width;

    if (end_index >= line_size)
        end_index = line_size;

    char* buffer = malloc(screen_width + 1);
    memset(buffer, ' ', screen_width);
    buffer[screen_width] = 0;

    if (pos_x < line_size)
        strncpy(buffer, line + start_index, end_index - start_index);
    
    DWORD written;
    WriteConsoleOutputCharacterA(STD, buffer, screen_width, (COORD){0, screen_line}, &written);
    free(buffer);
}

void print_file_content(const char* content, const Screen_info* screen_info, int pos_x, int text_line)
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
            print_line(content, line_size, screen_width, pos_x, screen_line);
            break;
        }

        int line_size = next_line - content;
        print_line(content, line_size, screen_width, pos_x, screen_line);

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

int count_lines(const char* file_content)
{
    int lines = 0;
    char* line_start = (char*)file_content;
    while (*line_start != 0)
    {
        char* line_end = strstr(line_start, "\n");
        if (line_end == NULL)
        {
            ++lines;
            return lines;
        }

        ++lines;
        line_start = line_end + 1;
    }

    return lines + 1;
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
    int line_count = count_lines(content);

    int pos_x = 0, pos_y = 0;
    int text_line = 0;
    INPUT_RECORD input_record;
    DWORD events;

    system("cls");
    for (;;)
    {
        
        print_file_content(content, &screen_info, pos_x, text_line);
        SetConsoleCursorPosition(STD, (COORD){pos_x, pos_y});

        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events);
        if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) 
        {
            switch (input_record.Event.KeyEvent.wVirtualKeyCode) 
            {
                case VK_UP:
                {
                    if (pos_y > 0)
                        --pos_y;
                    else if (text_line > 0)
                        --text_line;

                    break;
                }
                case VK_DOWN:
                {
                    if (pos_y == line_count - 1)
                        break;
                    if (pos_y < screen_info.height - 1)
                        ++pos_y;
                    else
                    {
                        if (text_line < line_count - 1)
                            ++text_line;
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
                    pos_x++;
                    break;
                case VK_ESCAPE:
                    goto end;
            }
        }
    }

end:
    system("cls");
    free(content);
    return 0;
}
