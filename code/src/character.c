#include "../headers/character.h"
#include <stdlib.h>
#include <string.h>

int length_of_characters(const char* string, int character_count)
{
    if (character_count == 0)
        return 0;
        
    int byte_count = 0;
    int characters_processed = 0;

    // iterate over bytes
    for (int byte_index = 0; string[byte_index] != '\0'; ++byte_index)
    {
        unsigned char c = string[byte_index];

        if ((c & 0x80) == 0) // 1 byte: 0xxxxxxx
            byte_count += 1;

        else if ((c & 0xE0) == 0xC0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            byte_count += 2;
            ++byte_index; // Skip the next byte of the character.
        }
        else if ((c & 0xF0) == 0xE0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            byte_count += 3;
            byte_index += 2; // Skip next 2 bytes of the character.
        }
        else if ((c & 0xF8) == 0xF0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            byte_count += 4;
            byte_index += 3; // Skip next 3 bytes of the character.
        }
        else
            return -1; // Invalid UTF-8 character.

        ++characters_processed;

        if (characters_processed == character_count)
            break;        
    }

    return byte_count;
}

int count_of_characters(const char* string)
{
    int character_count = 0;

    // iterate over bytes
    for (int byte_index = 0; string[byte_index] != '\0'; ++byte_index)
    {
        unsigned char c = string[byte_index];

        if ((c & 0x80) == 0) // 1 byte: 0xxxxxxx
        {
            ++character_count;
        }
        else if ((c & 0xE0) == 0xC0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            ++character_count;
            ++byte_index; // Skip 1 byte.
        }
        else if ((c & 0xF0) == 0xE0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            ++character_count;
            byte_index += 2; // Skip 2 bytes.
        }
        else if ((c & 0xF8) == 0xF0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            ++character_count;
            byte_index += 3; // Skip 3 bytes.
        }
        else
        {
            // Invalid UTF8 byte.
            return -1;
        }
    }

    return character_count;
}


char* get_characters_range(const char* string, int lower, int upper)
{
    // 1 byte: 0xxxxxxx
    // 2 bytes: 110xxxxx 10xxxxxx
    // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
    // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

    if (lower < 0 || lower > upper)
        return NULL; // Invalid input.

    int first_byte = -1;
    int last_byte = -1;

    int character_index = 0;
    for (int byte_index = 0; string[byte_index] != 0 && (first_byte < 0 || last_byte < 0); ++character_index)
    {
        unsigned char c = string[byte_index];
        int current_char_size = -1;

        if ((c & 0x80) == 0) // 1 byte: 0xxxxxxx
        {
            current_char_size = 1;
        }
        else if ((c & 0xE0) == 0xC0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            current_char_size = 2;
        }
        else if ((c & 0xF0) == 0xE0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            current_char_size = 3;
        }
        else if ((c & 0xF8) == 0xF0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            current_char_size = 4;
        }

        if (current_char_size < 0)
            return NULL;
        
        if (character_index == lower)
            first_byte = byte_index;
        
        if (character_index == upper)
            last_byte = byte_index + current_char_size - 1;


        byte_index += current_char_size;
    }

    int range_size = last_byte - first_byte + 1;
    char* range = malloc(range_size + 1);
    memcpy(range, string + first_byte, range_size);
    range[range_size] = 0;

    return range;
}

