#include "input.h"
#include <windows.h>

int read_input()
{
    static INPUT_RECORD input_record;
    static DWORD events;

    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events);
    
    if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown)
    {
        int input_key = input_record.Event.KeyEvent.wVirtualKeyCode;
        return input_key;
    }

    return 0;
}
