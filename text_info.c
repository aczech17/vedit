#include "text_info.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PUSH_LINE(line_size) if (!push_line_size(text_info, line_size)) { free_text_info(text_info); return NULL;}

static bool resize(Text_info* text_info)
{
    int new_capacity = 2 * text_info->capacity;
    int* new_line_sizes = malloc(new_capacity * sizeof(int));
    if (new_line_sizes == NULL)
        return false;

    memcpy(new_line_sizes, text_info->line_sizes, text_info->line_count * sizeof(int));
    free(text_info->line_sizes);
    text_info->line_sizes = new_line_sizes;
    text_info->capacity = new_capacity;

    return true;
}

static bool push_line_size(Text_info* text_info, int line_size)
{
    if (text_info->line_count == text_info->capacity)
    {
        bool correct = resize(text_info);
        if (!correct)
            return false;
    }

    text_info->line_sizes[text_info->line_count] = line_size;
    ++text_info->line_count;

    return true;
}

Text_info* get_text_info(const char* file_content)
{
    const int initial_capacity = 25;
    if (*file_content == 0) // empty file
    {
        return NULL;
    }

    Text_info* text_info = malloc(sizeof(Text_info));
    if (text_info == NULL)
        return NULL;

    text_info->line_count = 0;
    text_info->capacity = initial_capacity;

    text_info->line_sizes = malloc(text_info->capacity * sizeof(int));
    if (text_info->line_sizes == NULL)
    {
        free(text_info);
        return NULL;
    }

    char* line_start = (char*)file_content;
    while (*line_start != 0)
    {
        char* line_end = strstr(line_start, "\n");
        if (line_end == NULL)
        {
            // no empty line at the end
            int line_size = strlen(line_start);
            PUSH_LINE(line_size);
            
            return text_info;
        }

        int line_size = line_end - line_start;
        PUSH_LINE(line_size);

        line_start = line_end + 1;
    }

    PUSH_LINE(0);   // empty line at the end
    return text_info;
}

void free_text_info(Text_info* text_info)
{
    if (text_info == NULL)
        return;

    free(text_info->line_sizes);
    free(text_info);
}
