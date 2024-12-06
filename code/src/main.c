#include <stdbool.h>
#include <stdio.h>
#include "../headers/console_utils.h"
#include "../headers/display.h"
#include "../headers/input.h"
#include "../headers/text.h"
#include "../headers/update.h"
#include "../headers/view.h"

// typedef enum
// {
//     SUCCESS, ERROR, CANCEL
// }Saving_status;

// char* read_output_path(const Text* text, View* view, Key_code key)
// {
    
//     if (!text->modified)
//         return SUCCESS;
    
//     static char output_file_path[100] = {0};
//     static char* path_end = output_file_path;

//     *path_end++ = (char)key.value;

//     return output_file_path;
// }

int main(int argc, char** argv)
{
    int exit_status = 0;
    char exit_message[1025] = {0};

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
    char output_path[1025] = {0};
    char* output_end = output_path;

    while (running)
    {
        display_text(text, &view);
        display_log(text, &view, mode, output_path);
        set_cursor_position(view.cursor_x % view.screen_width, view.cursor_y);

        Key_code input_key = read_input();  // blocking

        // change mode if needed
        switch (mode)
        {
            case WATCH:
            {
                if (input_key.value == 'I' || input_key.value == 'i')
                {
                    mode = EDIT;
                    continue;
                }
                if (input_key.key_type == ESCAPE)
                {
                    mode = SAVE;
                    continue; // ???
                }
                break;
            }
            case EDIT:
            {
                if (input_key.key_type == ESCAPE)
                {
                    mode = WATCH;
                    continue; // ???
                    break;
                }
                break;
            }
            case SAVE:
            {
                if (input_key.key_type == ESCAPE)
                {
                    mode = WATCH;
                    continue;
                }
                if (input_key.key_type == F1)
                {
                    running = false;
                    continue;
                }
                if (input_key.key_type == ENTER)
                {
                    save_text(text, output_path); // TODO check save status
                    running = false;
                    continue;
                }

                break;
            }
        }

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
            case SAVE:
            {
                if (input_key.key_type == ALPHANUMERIC)
                {
                    *output_end++ = (char)input_key.value;
                }
                // output_path = read_output_path(text, &view, input_key);
                // continue;
                
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
