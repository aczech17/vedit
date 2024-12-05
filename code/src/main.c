#include <stdbool.h>
#include <stdio.h>
#include "../headers/console_utils.h"
#include "../headers/display.h"
#include "../headers/input.h"
#include "../headers/text.h"
#include "../headers/update.h"
#include "../headers/view.h"

int main(int argc, char** argv)
{
    int exit_status = 0;

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

    Mode mode = WATCH;

    clear_screen();
    bool running = true;
    while (running)
    {
        display_text(text, &view);
        display_log(text, &view, mode);
        set_cursor_position(view.cursor_x % view.screen_width, view.cursor_y);

        Key_code input_key = read_input();  // blocking

        switch (mode)
        {
            case WATCH:
            {
                if (input_key.value == 'I')
                    mode = EDIT;
                if (input_key.key_type == ESCAPE)
                    running = false;
                break;
            }
            case EDIT:
            {
                if (input_key.key_type == ESCAPE)
                    mode = WATCH;

                update_text(text, &view, input_key);
                break;
            }
        }
        
        update_view(text, &view, input_key);
    }

    // if (text->modified)
    {
        if (!save_text(text, "dupa.txt"))
        {
            fprintf(stderr, "Could not save the file.\n");
            exit_status = 4;
        }
    }

    deallocate_text(text);
    console_cleanup();
    return exit_status;
}
