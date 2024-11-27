#include "input.h"
#include <windows.h>

int read_input(const Text* text, Display_info* display_info)
{
    INPUT_RECORD input_record;
    DWORD events;

    int current_text_line = display_info->first_text_line + display_info->cursor_y;
    int current_text_line_size = strlen(text->lines[current_text_line]);
    if (display_info->cursor_x > current_text_line_size)
        display_info->cursor_x = current_text_line_size;

    int input_key = 0;
    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events);
    
    if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown)
    {
        input_key = input_record.Event.KeyEvent.wVirtualKeyCode;
        switch (input_key)
        {
        case VK_UP:
        {
            if (display_info->cursor_y > 0)
                --display_info->cursor_y;
            else if (display_info->first_text_line > 0)
                --display_info->first_text_line;

            break;
        }
        case VK_DOWN:
        {
            if (display_info->cursor_y >= text->line_count - 1)
                break;
            if (display_info->cursor_y < display_info->screen_height - 1)
                ++display_info->cursor_y;
            else
            {
                if (display_info->first_text_line < text->line_count - 1)
                    ++display_info->first_text_line;
            }

            break;
        }
        case VK_LEFT:
            if (display_info->cursor_x > 0)
            {
                display_info->cursor_x--;
            }
            break;
        case VK_RIGHT:
        {
            if (display_info->cursor_x == current_text_line_size)
                break;
            display_info->cursor_x++;
            break;
        }
        case VK_HOME:
            display_info->cursor_x = 0;
            break;
        case VK_END:
            display_info->cursor_x = current_text_line_size;
            break;
        }
    }

    return input_key;
}
