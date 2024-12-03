

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text.h"
#include "view.h"
#include "display.h"
#include "input.h"
#include "console_utils.h"


void update_text(Text* text, View* view, Key_code input_key)
{
    int current_text_line = view->first_text_line + view->cursor_y;

    switch (input_key.key_type)
    {
        case ALPHANUMERIC:
        {
            push_character(text, current_text_line, view->cursor_x, (char)input_key.value);
            view->cursor_x++;
            break;
        }
        case ENTER:
        {
            split_lines(text, current_text_line, view->cursor_x);
            view->cursor_y++;
            view->cursor_x = 0;
            break;
        }
        case BACKSPACE:
        {
            int next_cursor_x, next_cursor_y;

            if (view->cursor_x > 0) // stay on the current line
            {
                next_cursor_x = view->cursor_x - 1;
                next_cursor_y = view->cursor_y;
            }
            else    // Current line to be deleted
            {
                if (current_text_line > 0)
                {
                    char* previous_line = text->lines[current_text_line - 1];
                    next_cursor_x = strlen(previous_line);
                    next_cursor_y = view->cursor_y - 1;
                }
                else // We are on the beginning of the text. Don't change.
                {
                    next_cursor_x = view->cursor_x;
                    next_cursor_y = view->cursor_y;
                }

                // if (view->first_text_line + view->text_height - 1 >= text->line_count - 1)
                // {
                //     int line_to_clear = text->line_count % view->text_height - 1;
                //     clear_line(view, line_to_clear);
                // }
                clear_screen();
            }
            delete_character(text, current_text_line, view->cursor_x - 1);
   
            view->cursor_x = next_cursor_x;
            view->cursor_y = next_cursor_y;

            break;
        }
        default:
            break;
    }
}

void update_view(const Text* text, View* view, Key_code input_key)
{
    int current_text_line = view->first_text_line + view->cursor_y;
    int current_text_line_size = strlen(text->lines[current_text_line]);

    switch (input_key.key_type)
    {
        case ALPHANUMERIC:
            return;
        case DOWN:
        {
            view->cursor_y++;
            break;
        }
        case UP:
        {
            view->cursor_y--;
            break;
        }
        case LEFT:
        {
            view->cursor_x--;
            break;
        }
        case RIGHT:
        {
            view->cursor_x++;
            break;
        }
        case HOME:
        {
            view->cursor_x = 0;
            break;
        }
        case END:
        {
            view->cursor_x = current_text_line_size;
            break;
        }
        default:
            break;
    }

    if (view->cursor_x < 0)
    {
        view->cursor_x = 0;
    }

    if (view->cursor_x >= current_text_line_size)
        view->cursor_x = current_text_line_size;

    if (current_text_line == text->line_count)
        view->cursor_y--;

    if (view->cursor_y == view->text_height)
    {
        int new_last_line = view->first_text_line + view->text_height - 1;
        if (new_last_line < text->line_count - 1)
            view->first_text_line++;
        view->cursor_y--;
    }

    if (view->cursor_y < 0)
    {
        if (view->first_text_line > 0)
            view->first_text_line--;

        view->cursor_y = 0;
    }
}

int main(int argc, char** argv)
{
    console_setup();   
    View view = get_view();
    
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
 
    clear_screen();
    for (;;)
    {
        Key_code input_key = read_input();
        if (input_key.key_type == ESCAPE)
            break;

        update_text(text, &view, input_key);
        update_view(text, &view, input_key);
        display_text(text, &view);
        display_log(text, &view);
    }

    free_text(text);
    console_cleanup();
    return 0;
}
