#ifndef TEXT_INFO_H
#define TEXT_INFO_H

typedef struct
{
    int line_count;
    int* line_sizes;
    int capacity;
}Text_info;

Text_info* get_text_info(const char* file_content);
void free_text_info(Text_info* text_info);

#endif // TEXT_INFO_H
