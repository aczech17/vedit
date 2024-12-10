#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../headers/console_utils.h"
#include "../headers/display.h"
#include "../headers/input.h"
#include "../headers/text.h"
#include "../headers/update.h"
#include "../headers/view.h"

Mode get_new_mode(Mode mode, Character input_key, bool saving_needed)
{
    switch (mode)
    {
        case WATCH:
        {
            if (input_key.bytes[0] == 'I' || input_key.bytes[0] == 'i')
                return EDIT;
            if (input_key.character_type == ESCAPE)
            {
                if (saving_needed)
                    return READ_PATH;

                return QUIT; // If no saving is needed, just quit without saving.
            }
            
            break;
        }
        case EDIT:
        {
            if (input_key.character_type == ESCAPE)
                return WATCH;

            break;
        }
        case READ_PATH:
        {
            if (input_key.character_type == ESCAPE)
                return WATCH;
            if (input_key.character_type == ENTER)
                return SAVE;
            if (input_key.character_type == F1)
                return QUIT;

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

    for (;;)
    {
        display_text(text, &view);
        display_log(text, &view, mode, output_path);

        // Update program view before getting a key.
        switch (mode)
        {
            case WATCH:
            case EDIT:
            {
                // Discard the input from user if any.
                memset(output_path, 0, 1025); 
                output_end = output_path;
                
                set_cursor_position(view.cursor_x % view.screen_width, view.cursor_y);
                break;
            }
            case READ_PATH:
            {
                set_cursor_position(output_end - output_path, view.text_height + 1);
                break;
            }
            case SAVE:
            {
                bool save_success = save_text(text, output_path);
                if (!save_success)
                {
                    sprintf(exit_message, "Could not save the file %s.", output_path);
                    exit_status = 4;
                }

                mode = QUIT;
                break;
            }
            case QUIT:
            {
                // Do nothing, just end the main loop and quit.
                break;
            }
        }
        
        if (mode == QUIT)
            break;

        Character input_key = read_input(); // blocking
        Mode new_mode = get_new_mode(mode, input_key, text->modified);
        if (new_mode != mode && (mode == WATCH || mode == EDIT))
        {
            // If the mode has changed from watch to edit or from edit to wath, then read the input once again,
            // so that the last input key, changing the mode, doesn't affect the text.
            mode = new_mode;
            continue;
        }

        mode = new_mode;
    
        switch (mode)
        {
            case EDIT:
            {
                update_text(text, &view, input_key);
                break;
            }
            case READ_PATH:
            {
                if (input_key.character_type == ALPHANUMERIC)
                {
                    memcpy(output_end, input_key.bytes, input_key.size);
                    output_end += input_key.size;
                }

                if (input_key.character_type == BACKSPACE)
                    *(--output_end) = 0;
                
                break;
            }
            default:
                break;
        }

        update_view(text, &view, input_key);
    }

    deallocate_text(text);
    console_cleanup();

    if (exit_status != 0)
        fprintf(stderr, "%s", exit_message);

    return exit_status;
}
