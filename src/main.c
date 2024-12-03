#include <stdio.h>
#include "console_utils.h"
#include "display.h"
#include "input.h"
#include "text.h"
#include "update.h"
#include "view.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Filename missing.\n");
        return 1;
    }

    console_setup();   
    View view = get_view();
    
    char* filename = argv[1];
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        console_cleanup();
        fprintf(stderr, "Cannot open the file.\n");
        return 2;
    }

    Text* text = get_text(file);
    fclose(file);

    if (text == NULL)
    {
        console_cleanup();
        fprintf(stderr, "Could not parse the file.\n");
        return 3;
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
    }

    free_text(text);
    console_cleanup();
    return 0;
}
