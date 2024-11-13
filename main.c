#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

typedef struct
{
    int width;
    int height;
}Screen_info;

Screen_info get_screen_info()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(std_out, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    Screen_info info = {.width = width, .height = height};
    return info;
}

void print_line(const char* line, int line_size, int screen_width, int pos_x)
{
    if (pos_x >= line_size)
    {
        printf("\n");
        return;
    }

    int start_index = (pos_x / screen_width) * screen_width;
    int end_index = start_index + screen_width;

    if (end_index >= line_size)
        end_index = line_size;

    printf("%.*s\n", end_index - start_index, line + start_index);
}

void print_file_content(const char* content, const Screen_info* screen_info, int pos_x, int pos_y)
{
    int line_count = screen_info->height - 5;
    int screen_width = screen_info->width;

    int line_number = 0;
    // ommit pos_y lines
    while (line_number < pos_y)
    {
        const char* next_line = strstr(content, "\n");
        if (next_line == NULL)
            return;
        
        content = next_line + 1;
        ++line_number;
    }

    line_number = 0;
    while (line_number < line_count)
    {
        const char* next_line = strstr(content, "\n");
        if (next_line == NULL)
        {
            int line_size = strlen(content);
            print_line(content, line_size, screen_width, pos_x);
            break;
        }

        int line_size = next_line - content;
        print_line(content, line_size, screen_width, pos_x);

        content = next_line + 1;
        ++line_number;
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
    Screen_info screen_info = get_screen_info();

    if (argc < 2)
    {
        fprintf(stderr, "Nazwa pliku.\n");
        return 1;
    }

    char* filename = argv[1];
    char* content = get_file_content(filename);

    int pos_x = 0, pos_y = 0;

    for (;;)
    {
        system("cls");

        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            --pos_y; // down
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            ++pos_y; // up
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            --pos_x; // left
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            ++pos_x; // right
        }

        print_file_content(content, &screen_info, pos_x, pos_y);
        printf("%d %d", pos_x, pos_y);

        Sleep(50);
    }

    free(content);
    return 0;
}
