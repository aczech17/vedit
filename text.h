#ifndef TEXT_H
#define TEXT_H

typedef struct
{
    char** lines;
    int* line_sizes;
    int line_count;
    int capacity;
}Text;

Text* get_text(const char* file_content);
void free_text(Text* text);

#endif // TEXT_H
