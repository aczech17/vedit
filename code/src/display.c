#include "../headers/display.h"
#include "../headers/view.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <stdio.h>
#endif

void set_cursor_position(int x, int y)
{
    #ifdef _WIN32
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){x, y});
    #elif __linux__
        printf("\033[?25h"); // Set the cursor on.
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

static void print_line(const char* line, int screen_width, int cursor_x, int screen_line)
{
    int line_size = strlen(line);
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

void display_text(const Text* text, const View* view)
{
    #ifdef __linux__
        printf("\033[?25l"); // Hide the cursor not to blink randomly while printing the text.
        fflush(stdout);
    #endif

    int line_count = text->line_count < view->text_height ? text->line_count : view->text_height;
    int screen_width = view->screen_width;

    for (int screen_line = 0; screen_line < line_count; ++screen_line)
    {
        int text_line_number = screen_line + view->first_text_line;
        if (text_line_number >= text->line_count)
            break;
            
        char* line = text->lines[text_line_number];
        print_line(line, screen_width, view->cursor_x, screen_line);
    }
}

void clear_line(const View* view, int screen_line)
{
    char* empty_line = malloc(view->screen_width + 1);
    memset(empty_line, ' ', view->screen_width);
    empty_line[view->screen_width] = 0;

    print_line(empty_line, view->screen_width, 0, screen_line);
    free(empty_line);
}

void display_log(const Text* text, const View* view)
{
    char* log = malloc(view->screen_width + 1);
    memset(log, ' ', view->screen_width);
    log[view->screen_width] = 0;

    int selected_text_line = view->first_text_line + view->cursor_y;
    sprintf(log, "%d, %d \t %d lines \t from %d", view->cursor_x, selected_text_line, text->line_count, view->first_text_line);
    write_line_to_console(log, view->screen_width, view->text_height);

    memset(log, ' ', view->screen_width);
    sprintf(log, "cursor: %d, %d", view->cursor_x, view->cursor_y);
    write_line_to_console(log, view->screen_width, view->text_height + 1);

    free(log);
}
