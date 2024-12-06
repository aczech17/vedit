#include <stdbool.h>
#include <stdio.h>
#include "../headers/console_utils.h"
#include "../headers/display.h"
#include "../headers/input.h"
#include "../headers/text.h"
#include "../headers/update.h"
#include "../headers/view.h"

Mode get_new_mode(Mode mode, Key_code input_key, bool saving_needed)
{
    switch (mode)
    {
        case WATCH:
        {
            if (input_key.value == 'I' || input_key.value == 'i')
                return EDIT;
            if (input_key.key_type == ESCAPE)
            {
                if (saving_needed)
                    return SAVING;

                return DISCARD_AND_QUIT; // If no saving is needed, just quit without saving.
            }
            
            break;
        }
        case EDIT:
        {
            if (input_key.key_type == ESCAPE)
                return WATCH;

            break;
        }
        case SAVING:
        {
            if (input_key.key_type == ESCAPE)
                return WATCH;
            if (input_key.key_type == ENTER)
                return SAVE_AND_QUIT;
            if (input_key.key_type == F1)
                return DISCARD_AND_QUIT;

            break;
        }
        default:
            break;
    }

    return mode; // Otherwise, dont't change anything.
}

int main(int argc, char** argv)
{
    int exit_status = 0;
    char exit_message[2049] = {0};

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
    char output_path[1025] = {0};
    char* output_end = output_path;

    while (mode != SAVE_AND_QUIT && mode != DISCARD_AND_QUIT)
    {
        display_text(text, &view);
        display_log(text, &view, mode, output_path);
        set_cursor_position(view.cursor_x % view.screen_width, view.cursor_y);

        Key_code input_key = read_input(); // blocking
        Mode new_mode = get_new_mode(mode, input_key, text->modified);
        if (new_mode != mode)
        {
            // If the mode has changed, read the input once again,
            // so that the last input key, changing the mode, doesn't affect the text.
            mode = new_mode;
            continue;
        }

        mode = new_mode;
    
        switch (mode)
        {
            case WATCH:
            {
                break;
            }
            case EDIT:
            {
                update_text(text, &view, input_key);
                break;
            }
            case SAVING:
            {
                if (input_key.key_type == ALPHANUMERIC)
                    *output_end++ = (char)input_key.value; // Read output path.

                break;
            }
            case SAVE_AND_QUIT:
            {
                bool save_success = save_text(text, output_path);
                if (!save_success)
                {
                    sprintf(exit_message, "Could not save the file %s.", output_path);
                    exit_status = 4;
                }
                break;
            }
            case DISCARD_AND_QUIT:
            {
                // Do nothing, just end the main loop.
                break;
            }
        }

        update_view(text, &view, input_key);
    }

    deallocate_text(text);
    console_cleanup();

    if (exit_status != 0)
        fprintf(stderr, "%s", exit_message);

    return exit_status;
}
