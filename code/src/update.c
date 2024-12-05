#include "../headers/update.h"
#include <string.h>
#include "../headers/console_utils.h"

void update_text(Text* text, View* view, Key_code input_key)
{
    int current_text_line = view->first_text_line + view->cursor_y;

    bool modified_now = true;
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

                clear_screen(); // Could be better.
            }
            delete_character(text, current_text_line, view->cursor_x - 1);
   
            view->cursor_x = next_cursor_x;
            view->cursor_y = next_cursor_y;

            break;
        }
        default:
            modified_now = false;
            break;
    }

    // If it wasn't modified earlier, it's modified now.
    if (!text->modified)
        text->modified = modified_now;
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
        view->cursor_x = 0;

    if (view->cursor_y >= text->line_count)
        view->cursor_y--;

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
