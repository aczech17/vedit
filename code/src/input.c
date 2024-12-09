#include "../headers/input.h"

char* mode_to_str(const Mode mode)
{
    switch (mode)
    {
        case WATCH:
            return "WATCH";
        case EDIT:
            return "EDIT";
        case READ_PATH:
            return "SAVE";
        default:
            return "MODE UNKNOWN";
    }
}

static Key_code functional_key(Key_type key_type)
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

static Key_code convert_special_key_code_windows(int winapi_input_key, bool is_shift_pressed)
{
    switch (winapi_input_key)
    {
        case VK_UP:          return functional_key(UP);
        case VK_DOWN:        return functional_key(DOWN);
        case VK_LEFT:        return functional_key(LEFT);
        case VK_RIGHT:       return functional_key(RIGHT);
        case VK_HOME:        return functional_key(HOME);
        case VK_END:         return functional_key(END);
        case VK_RETURN:      return functional_key(ENTER);
        case VK_BACK:        return functional_key(BACKSPACE);
        case VK_ESCAPE:      return functional_key(ESCAPE);
        case VK_F1:          return functional_key(F1);

        case VK_OEM_PERIOD:  return alphanumeric(is_shift_pressed ? '>' : '.');
        case VK_OEM_COMMA:   return alphanumeric(is_shift_pressed ? '<' : ',');
        case VK_OEM_1:       return alphanumeric(is_shift_pressed ? ':' : ';');
        case VK_OEM_2:       return alphanumeric(is_shift_pressed ? '?' : '/');
        case VK_OEM_3:       return alphanumeric(is_shift_pressed ? '~' : '`');
        case VK_OEM_4:       return alphanumeric(is_shift_pressed ? '{' : '[');
        case VK_OEM_5:       return alphanumeric(is_shift_pressed ? '|' : '\\');
        case VK_OEM_6:       return alphanumeric(is_shift_pressed ? '}' : ']');
        case VK_OEM_7:       return alphanumeric(is_shift_pressed ? '"' : '\'');
        case VK_OEM_PLUS:    return alphanumeric(is_shift_pressed ? '+' : '=');
        case VK_OEM_MINUS:   return alphanumeric(is_shift_pressed ? '_' : '-');    
        case '1': return alphanumeric(is_shift_pressed ? '!' : '1');
        case '2': return alphanumeric(is_shift_pressed ? '@' : '2');
        case '3': return alphanumeric(is_shift_pressed ? '#' : '3');
        case '4': return alphanumeric(is_shift_pressed ? '$' : '4');
        case '5': return alphanumeric(is_shift_pressed ? '%' : '5');
        case '6': return alphanumeric(is_shift_pressed ? '^' : '6');
        case '7': return alphanumeric(is_shift_pressed ? '&' : '7');
        case '8': return alphanumeric(is_shift_pressed ? '*' : '8');
        case '9': return alphanumeric(is_shift_pressed ? '(' : '9');
        case '0': return alphanumeric(is_shift_pressed ? ')' : '0');
        case ' ': return alphanumeric(' ');

        default: return no_key();
    }
}

Key_code read_input()
{
    static INPUT_RECORD input_record;
    static DWORD events;
    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events);

    bool is_shift_pressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool is_caps_lock_on = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    
    if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown)
    {
        int input_key = input_record.Event.KeyEvent.wVirtualKeyCode;

        // First check if the key is special, because some special keys may appear as a regular ASCII,
        // eg. F1 and 'p'.
        Key_code converted = convert_special_key_code_windows(input_key, is_shift_pressed);
        if (converted.key_type != NONE)
            return converted;

        // Now we are sure the key is none of a special.

        if (!isalpha(input_key))    // No special key, no alpha, it's irrelevant.
            return no_key();

        bool uppercase = is_shift_pressed ^ is_caps_lock_on;
        if (!uppercase)
            input_key = tolower(input_key);
            
        return alphanumeric(input_key);
    }

    return no_key();
}

#elif __linux__
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

static Key_code convert_key_code_linux(const char* sequence)
{
    if (strcmp(sequence, "[A") == 0)
        return functional_key(UP);
    if (strcmp(sequence, "[B") == 0)
        return functional_key(DOWN);
    if (strcmp(sequence, "[C") == 0)
        return functional_key(RIGHT);
    if (strcmp(sequence, "[D") == 0)
        return functional_key(LEFT);
    if (strcmp(sequence, "[H") == 0)
        return functional_key(HOME);
    if (strcmp(sequence, "[F") == 0)
        return functional_key(END);
    if (strcmp(sequence, "OP") == 0)
        return functional_key(F1);
    if (strcmp(sequence, "[11~") == 0)
        return functional_key(F1);

    return no_key();
}

Key_code read_input()
{
    char buffer[5] = {0};
    ssize_t bytes_read = read(STDIN_FILENO, buffer, 1);

    if (bytes_read <= 0)
        return no_key();

    if (buffer[0] == '\033') // Escape sequence or standalone escape key.
    {
        // Peek to check if it's a standalone ESC key or a sequence.
        struct timeval timeout = {0, 10000};
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout) > 0)
        {
            if (read(STDIN_FILENO, buffer + 1, 4) >= 2) // There's more data, read it.
            {
                if (buffer[1] == '[' || buffer[1] == 'O')
                {
                    return convert_key_code_linux(buffer + 1);
                }
            }
        }
        else
        {
            return functional_key(ESCAPE); // No more data, it's a standalone ESC key.
        }
    }

    // Other ASCII chars
    switch (buffer[0])
    {
        case 127:
            return functional_key(BACKSPACE);
        case '\n':
            return functional_key(ENTER);
        case 27:
            return functional_key(ESCAPE);
        default:
            return alphanumeric(buffer[0]);
    }
}

#endif
