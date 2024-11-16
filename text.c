#include "text.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PUSH_LINE(line, line_size) if (!push_line(text, line, line_size)) {free_text(text); return NULL;}

static char* get_file_content(FILE* file)
{
    fseek(file, 0, SEEK_END);
    size_t file_size = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(file_size + 1);
    size_t bytes_read = fread(content, 1, file_size, file);
    if (bytes_read < file_size)
    {
        free(content);
        return NULL;
    }

    content[file_size] = 0;
    return content;
}

static bool resize(Text* text)
{
    int new_capacity = 2 * text->capacity;

    char** new_lines = malloc(new_capacity * sizeof(char*));
    if (new_lines == NULL)
        return false;
    
    memcpy(new_lines, text->lines, text->line_count * sizeof(char*));
    free(text->lines);
    text->lines = new_lines;



    int* new_line_sizes = malloc(new_capacity * sizeof(int));
    if (new_line_sizes == NULL)
    {
        free(text->lines);
        return false;
    }

    memcpy(new_line_sizes, text->line_sizes, text->line_count * sizeof(int));
    free(text->line_sizes);
    text->line_sizes = new_line_sizes;

    

    text->capacity = new_capacity;

    return true;
}

static bool push_line(Text* text, const char* line, int line_size)
{
    if (text->line_count == text->capacity)
    {
        bool correct = resize(text);
        if (!correct)
            return false;
    }

    char* new_line = malloc(line_size + 1);
    if (new_line == NULL)
        return false;
    memcpy(new_line, line, line_size);
    new_line[line_size] = 0;
    text->lines[text->line_count] = new_line;

    text->line_sizes[text->line_count] = line_size;

    ++text->line_count;

    return true;
}

Text* get_text(FILE* file)
{
    char* file_content = get_file_content(file);

    const int initial_capacity = 25;

    Text* text = malloc(sizeof(Text));
    if (text == NULL)
        return NULL;

    text->line_count = 0;
    text->capacity = initial_capacity;

    text->lines = malloc(text->capacity * sizeof(char*));
    if (text->lines == NULL)
    {
        free(text);
        return NULL;
    }

    text->line_sizes = malloc(text->capacity * sizeof(int));
    if (text->line_sizes == NULL)
    {
        free(text->lines);
        free(text);
        return NULL;
    }


    char* line_start = (char*)file_content;
    while (*line_start != 0)
    {
        char* line_end = strchr(line_start, '\n');
        if (line_end == NULL)
        {
            // no empty line at the end
            int line_size = strlen(line_start);
            PUSH_LINE(line_start, line_size);
            
            return text;
        }

        int line_size = line_end - line_start;
        push_line(text, line_start, line_size);

        line_start = line_end + 1;
    }

    PUSH_LINE("", 0)   // empty line at the end

    free(file_content);
    return text;
}

void free_text(Text* text)
{
    if (text == NULL)
        return;

    for (int i = 0; i < text->line_count; ++i)
        free(text->lines[i]);

    free(text->lines);
    free(text->line_sizes);
    free(text);
}
