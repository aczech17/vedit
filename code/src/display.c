#include "../headers/display.h"
#include "../headers/view.h"
#include <stdlib.h>
#include <string.h>
#include "../headers/character.h"

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

static void write_line_to_console(char* line, int screen_width, int screen_line)
{
    #ifdef _WIN32
        static DWORD written;
        WriteConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), line, screen_width, (COORD){0, screen_line}, &written);
    #elif __linux__
        printf("\033[%d;1H", screen_line + 1); // // ANSI escape sequence is 1-indexed.
        fwrite(line, 1, screen_width, stdout);
        fflush(stdout);
    #else
        #error "Writing to console for this OS is not implemented."
    #endif
}

void print_line(const char* line, int screen_width, int cursor_x, int screen_line)
{
    char* buffer = malloc(4 * screen_width + 1);
    memset(buffer, ' ', 4 * screen_width);
    buffer[4 * screen_width] = 0;

    int line_size = count_of_characters(line);

    // Calculate the range of characters to display.
    int start_index = (cursor_x / screen_width) * screen_width;
    int end_index = start_index + screen_width;
    if (end_index > line_size)
        end_index = line_size;

    char* line_content = get_characters_range(line, start_index, end_index - 1); // inclusive range

    if (line_content)
        strcpy(buffer, line_content);

    
    write_line_to_console(buffer, 4 * screen_width, screen_line);

    free(line_content);
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

// void clear_line(const View* view, int screen_line)
// {
//     char* empty_line = malloc(view->screen_width + 1);
//     memset(empty_line, ' ', view->screen_width);
//     empty_line[view->screen_width] = 0;

//     print_line(empty_line, view->screen_width, 0, screen_line);
//     free(empty_line);
// }

void display_log(const Text* text, const View* view, const Mode mode, const char* log_input)
{
    char* log = malloc(view->screen_width + 1);
    memset(log, ' ', view->screen_width);
    log[view->screen_width] = 0;

    if (mode == READ_PATH)
    {
        const char* save_prompt = "Write path to save (press F1 to discard and exit):";
        strcpy(log, save_prompt);
        write_line_to_console(log, view->screen_width, view->text_height);

        memset(log, ' ', view->screen_width);

        strcpy(log, log_input);
        write_line_to_console(log, view->screen_width, view->text_height + 1);

        set_cursor_position(strlen(log_input), view->text_height + 1);

        free(log);
        return;
    }
    
    
    int selected_text_line = view->first_text_line + view->cursor_y;
    int line_count;
    if (text->line_count == 1 && strlen(text->lines[0]) == 0)
        line_count = 0;
    else
        line_count = text->line_count;

    int current_line_size = count_of_characters(text->lines[selected_text_line]);
    int current_line_bytes = strlen(text->lines[selected_text_line]);
    sprintf(log, "%d, %d \t %d lines \t from %d. %d characters, %d bytes. MODE: %s",
            view->cursor_x, selected_text_line, line_count, view->first_text_line, current_line_size, current_line_bytes, mode_to_str(mode));
    write_line_to_console(log, view->screen_width, view->text_height);

    memset(log, ' ', view->screen_width);
    sprintf(log, "cursor: %d, %d. Screen width: %d", view->cursor_x, view->cursor_y, view->screen_width);
    write_line_to_console(log, view->screen_width, view->text_height + 1);

    free(log);
}
