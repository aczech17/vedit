#include <stdio.h>
#include "../headers/console_utils.h"
#include "../headers/display.h"
#include "../headers/input.h"
#include "../headers/text.h"
#include "../headers/update.h"
#include "../headers/view.h"

int main(int argc, char** argv)
{
    console_setup();   
    View view = get_view();

    Text* text;

    if (argc > 1)
    {
        char* filename = argv[1];
        FILE* file = fopen(filename, "rb");
        if (file == NULL)
        {
            console_cleanup();
            fprintf(stderr, "Cannot open the file.\n");
            return 1;
        }

        text = get_text_from_file(file);
        fclose(file);

        if (text == NULL)
        {
            console_cleanup();
            fprintf(stderr, "Could not parse the file.\n");
            return 2;
        }
    }
    else
    {
        text = empty_text();
        if (text == NULL)
        {
            fprintf(stderr, "Could not create a file.\n");
            return 3;
        }
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

        set_cursor_position(view.cursor_x % view.screen_width, view.cursor_y);
    }

    deallocate_text(text);
    console_cleanup();
    return 0;
}
