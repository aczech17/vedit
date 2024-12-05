#include "../headers/input.h"

char* mode_to_str(const Mode mode)
{
    switch (mode)
    {
        case WATCH:
            return "WATCH";
        case EDIT:
            return "EDIT";
        default:
            return "MODE UNKNOWN";
    }
}

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

static Key_code convert_special_key_code_windows(int winapi_input_key)
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
        case VK_ESCAPE:
            return special_key(ESCAPE);
        default:
            return no_key();
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
        bool is_alphanumeric = isalnum(input_key) || input_key == ' '; // Space is considered alphanumeric.
        if (!is_alphanumeric)
            return convert_special_key_code_windows(input_key);

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

    if (buffer[0] == '\033') // Escape sequence or Escape key
    {
        // Peek to check if it's a standalone ESC key or a sequence
        struct timeval timeout = {0, 10000}; // 10ms timeout
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout) > 0)
        {
            // There's more data, read it
            if (read(STDIN_FILENO, buffer + 1, 2) == 2 && buffer[1] == '[')
            {
                return convert_key_code_linux(buffer + 1);
            }
        }
        else
        {
            // No more data, it's a standalone ESC key
            return special_key(ESCAPE);
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
