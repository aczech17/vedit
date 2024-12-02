#include "input.h"

#ifdef __linux__
#include <string.h>
#include <unistd.h>
#endif

static Key_code special_key(Key_type key_type)
{
    return (Key_code){.key_type = key_type, .value = 0};
}

static Key_code no_key()
{
    return (Key_code){.key_type = NONE, .value = 0};
}

static Key_code alphanumeric(int value)
{
    return (Key_code){.key_type = ALPHANUMERIC, .value = value};
}

#ifdef _WIN32
#include <windows.h>

static Key_code convert_key_code_windows(int winapi_input_key)
{
    switch (winapi_input_key)
    {
        case VK_UP:
            return special_key(UP);
        case VK_DOWN:
            return special_key(DOWN);
        case VK_LEFT:
            return special_key(LEFT);
        case VK_RIGHT:
            return special_key(RIGHT);
        case VK_HOME:
            return special_key(HOME);
        case VK_END:
            return special_key(END);
        case VK_RETURN:
            return special_key(ENTER);
        case VK_BACK:
            return special_key(BACKSPACE);
        case VK_ESCAPE: // Obsługuje ESCAPE
            return special_key(ESCAPE);
        default:
        {
            if (isalnum(winapi_input_key))
                return alphanumeric(winapi_input_key);
            return no_key();
        }
    }
}

Key_code read_input()
{
    static INPUT_RECORD input_record;
    static DWORD events;

    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events);
    
    if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown)
    {
        int winapi_input_key = input_record.Event.KeyEvent.wVirtualKeyCode;
        Key_code key_code = convert_key_code_windows(winapi_input_key);
        return key_code;
    }

    return no_key();
}

#elif __linux__

static Key_code convert_key_code_linux(const char* sequence)
{
    if (strcmp(sequence, "[A") == 0)
        return special_key(UP);
    if (strcmp(sequence, "[B") == 0)
        return special_key(DOWN);
    if (strcmp(sequence, "[C") == 0)
        return special_key(RIGHT);
    if (strcmp(sequence, "[D") == 0)
        return special_key(LEFT);
    if (strcmp(sequence, "[H") == 0)
        return special_key(HOME);
    if (strcmp(sequence, "[F") == 0)
        return special_key(END);

    return no_key();
}

Key_code read_input()
{
    char buffer[3] = {0};
    ssize_t bytes_read = read(STDIN_FILENO, buffer, 1);

    if (bytes_read <= 0)
        return no_key();

    if (buffer[0] == '\033') // Escape sequence
    {
        if (read(STDIN_FILENO, buffer + 1, 2) == 2 && buffer[1] == '[')
        {
            return convert_key_code_linux(buffer + 1);
        }

        return no_key();
    }

    // Other ASCII chars
    switch (buffer[0])
    {
        case 127: // Backspace
            return special_key(BACKSPACE);
        case '\n': // Enter
            return special_key(ENTER);
        case 27:
            return special_key(ESCAPE);
        default: // Alphanumeric
            return alphanumeric(buffer[0]);
    }
}

#endif
