#include "../headers/text.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    char* start;
    int size;
}Endline;

#define PUSH_LINE(line, line_size) if (!push_line(text, line, line_size)) {deallocate_text(text); return NULL;}

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

    ++text->line_count;

    return true;
}

static Endline find_endline(const char* line)
{
    const int CR = 0xD;
    const int LF = 0xA;

    char* cr_pos = strchr(line, CR);
    char* lf_pos = strchr(line, LF);

    char* start;
    int size;
    
    if (cr_pos && lf_pos)
    {
        start = cr_pos;
        size = 2;
    }
    if (cr_pos && !lf_pos)
    {
        start = cr_pos;
        size = 1;
    }
    if (!cr_pos && lf_pos)
    {
        start = lf_pos;
        size = 1;
    }
    if (!cr_pos && !lf_pos)
    {
        start = NULL;
        size = 0;
    }

    Endline endline =
    {
        .start = start,
        .size = size,
    };

    return endline;
}

static Text* new_text()
{
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

    return text;
}

Text* empty_text()
{
    Text* text = new_text();
    if (text == NULL)
        return NULL;
        
    PUSH_LINE("", 0); // only empty line

    return text;
}

Text* get_text_from_file(FILE* file)
{
    char* file_content = get_file_content(file);
    Text* text = new_text();


    char* line_start = (char*)file_content;
    while (*line_start != 0)
    {
        Endline endline = find_endline(line_start);
        if (endline.start == NULL)
        {
            // no empty line at the end
            int line_size = strlen(line_start);
            PUSH_LINE(line_start, line_size);
            
            return text;
        }

        int line_size = endline.start - line_start;
        push_line(text, line_start, line_size);

        line_start = endline.start + endline.size;
    }

    PUSH_LINE("", 0)   // empty line at the end

    free(file_content);
    return text;
}

void push_character(Text* text, int line_number, int char_position, char value)
{
    if (line_number >= text->line_count)
        return;

    char* line = text->lines[line_number];
    int line_size = strlen(line);
    char* new_line = malloc(line_size + 2);

    for (int i = 0; i < char_position; ++i)
        new_line[i] = line[i];

    new_line[char_position] = value;

    for (int i = char_position; i < line_size; ++i)
        new_line[i + 1] = line[i];
    new_line[line_size + 1] = 0;
    
    free(text->lines[line_number]);
    text->lines[line_number] = new_line;
}

void delete_character(Text* text, int line_number, int char_position)
{
    if (char_position == -1)
    {
        if (line_number > 0)
            join_lines(text, line_number - 1);
        // else
        return;
    }
    
    char* line = text->lines[line_number];
    int old_line_size = strlen(line);
    char* new_line = malloc(old_line_size); // -1 because of deletion and + 1 because of '\0'. -1 + 1 = 0

    for (int i = 0; i < char_position; ++i)
        new_line[i] = line[i];
    
    for (int i = char_position + 1; i < old_line_size; ++i)
        new_line[i - 1] = line[i];

    new_line[old_line_size - 1] = 0; // Size of the buffer is old_line_size, so the terminator index is old_line_size - 1.

    free(line);
    text->lines[line_number] = new_line;
}

void delete_line(Text* text, int line_number)
{
    if (line_number >= text->line_count)
        return;
        
    free(text->lines[line_number]);

    for (int i = line_number; i < text->line_count - 1; ++i)
        text->lines[i] = text->lines[i + 1];

    --text->line_count;
}

void split_lines(Text* text, int line_number, int split_position)
{
    char* line = text->lines[line_number];

    char* first_half = malloc(split_position + 1);
    memcpy(first_half, line, split_position);
    first_half[split_position] = 0;

    int second_half_size = strlen(line) - split_position + 1;
    char* second_half = malloc(second_half_size);
    memcpy(second_half, line + split_position, second_half_size); // We're also copying the '\0' here.

    // Replace the line with the first half.
    free(text->lines[line_number]);
    text->lines[line_number] = first_half;

    // The second half should be on the position line_number + 1.

    // Now push the second half to the end.
    push_line(text, second_half, second_half_size);
    free(second_half);  // The copy has been made.

    // The memory is already allocated.
    // Now we have to move the pointers.
    // Move down to line_number + 2.
    char* new_line = text->lines[text->line_count - 1];
    for (int i = text->line_count - 1; i >= line_number + 2; --i)
        text->lines[i] = text->lines[i - 1];
    text->lines[line_number + 1] = new_line;
}

void join_lines(Text* text, int upper_line_number)
{
    char* upper_line = text->lines[upper_line_number];
    int upper_line_size = strlen(upper_line);

    char* lower_line = text->lines[upper_line_number + 1];
    int lower_line_size = strlen(lower_line);

    int new_line_size = upper_line_size + lower_line_size;
    char* new_line = malloc(new_line_size + 1);
    memcpy(new_line, upper_line, upper_line_size);
    memcpy(new_line + upper_line_size, lower_line, lower_line_size);
    new_line[new_line_size] = 0;

    free(text->lines[upper_line_number]);
    free(text->lines[upper_line_number + 1]);
    text->lines[upper_line_number] = new_line;

    for (int i = upper_line_number + 1; i < text->line_count - 1; ++i)
        text->lines[i] = text->lines[i + 1];
    
    text->line_count--;
}

void deallocate_text(Text* text)
{
    if (text == NULL)
        return;

    for (int i = 0; i < text->line_count; ++i)
        free(text->lines[i]);

    free(text->lines);
    free(text);
}
