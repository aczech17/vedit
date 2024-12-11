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

static Character functional_key(Character_type character_type)
{
    return (Character){.character_type = character_type, .bytes = {0}, .size = 1};
}

static Character no_key()
{
    return (Character){.character_type = NONE, .bytes = {0}, .size = 0};
}

#ifdef _WIN32
#include <windows.h>
#include <string.h>

static Character convert_special_key_code_windows(int winapi_input_key)
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
        default: return no_key();
    }
}

Character read_input()
{
    static INPUT_RECORD input_record[128];
    static DWORD events;
    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), input_record, 128, &events);
    

    // First check if special.
    if (input_record[0].EventType == KEY_EVENT && input_record[0].Event.KeyEvent.bKeyDown)
    {
        int input_key = input_record[0].Event.KeyEvent.wVirtualKeyCode;

        Character special_key = convert_special_key_code_windows(input_key);
        if (special_key.character_type != NONE)
            return special_key;
    }

    // No special key, it's a unicode character.
    char bytes[4] = {0};
    int bytes_count = 0;

    for (DWORD i = 0; i < events; ++i)
    {
        if (input_record[i].EventType == KEY_EVENT && input_record[i].Event.KeyEvent.bKeyDown)
        {
            wchar_t pressed_char = input_record[i].Event.KeyEvent.uChar.UnicodeChar;
            if (pressed_char == 0)
                continue;

            bytes[bytes_count++] = (char)(pressed_char & 0xFF);
            if (pressed_char > 0xFF)
                bytes[bytes_count++] = (char)((pressed_char >> 8) & 0xFF);
        }
    }

    if (bytes_count == 0)
        return no_key();

    Character character =
    {
        .character_type = ALPHANUMERIC,
        .size = bytes_count,
    };
    memcpy(character.bytes, bytes, bytes_count);

    return character;
}

#elif __linux__
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

static Character convert_key_code_linux(const char* sequence)
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

Character read_input()
{
    char buffer[6] = {0};
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
            return ascii_character(buffer[0]);
    }
}

#endif
