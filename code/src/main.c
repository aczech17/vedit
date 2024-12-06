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
    char exit_message[1024] = {0};

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
                if (input_key.value == 'I' || input_key.value == 'i')
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

    char output_file_path[100] = {0};
    char* path_end = output_file_path;

    if (text->modified)
    {
        print_line("Write file to:", view.screen_width, 0, view.text_height);
        print_line("", view.screen_width, 0, view.text_height + 1);
        view.cursor_x = 0;
        set_cursor_position(view.cursor_x, view.text_height + 1);

        Key_code key = {.key_type = NONE};
        while (key.key_type != ENTER)
        {
            key = read_input();
            if (key.key_type == ALPHANUMERIC)
            {
                *path_end++ = (char)key.value;
                view.cursor_x++;
                print_line(output_file_path, view.screen_width, view.cursor_x, view.text_height + 1);
                set_cursor_position(view.cursor_x, view.text_height + 1);
            }
        }

        bool save_success = save_text(text, output_file_path);
        if (!save_success)
        {
            sprintf(exit_message, "Could not save the file %s.\n", output_file_path);
            exit_status = 4;
        }
    }

    deallocate_text(text);
    console_cleanup();

    if (exit_status != 0)
        fprintf(stderr, "%s", exit_message);

    return exit_status;
}
