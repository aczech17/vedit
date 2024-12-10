#include "../headers/character.h"

int length_of_characters(char* string, int character_count)
{
    if (character_count == 0)
        return 0;
        
    int byte_count = 0;
    int characters_processed = 0;

    // iterate over bytes
    for (int i = 0; string[i] != '\0'; ++i)
    {
        unsigned char c = string[i];

        if ((c & 0x80) == 0) // 1 byte: 0xxxxxxx
            byte_count += 1;

        else if ((c & 0xE0) == 0xC0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            byte_count += 2;
            ++i; // Skip the next byte of the character.
        }
        else if ((c & 0xF0) == 0xE0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            byte_count += 3;
            i += 2; // Skip next 2 bytes of the character.
        }
        else if ((c & 0xF8) == 0xF0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            byte_count += 4;
            i += 3; // Skip next 3 bytes of the character.
        }
        else
            return -1; // Invalid UTF-8 character.

        ++characters_processed;

        if (characters_processed == character_count)
            break;        
    }

    return byte_count;
}

int character_count_of_string(char* string)
{
    int character_count = 0;

    // iterate over bytes
    for (int i = 0; string[i] != '\0'; ++i)
    {
        unsigned char c = string[i];

        if ((c & 0x80) == 0) // 1 byte: 0xxxxxxx
        {
            ++character_count;
        }
        else if ((c & 0xE0) == 0xC0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            ++character_count;
            ++i; // Skip 1 byte.
        }
        else if ((c & 0xF0) == 0xE0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            ++character_count;
            i += 2; // Skip 2 bytes.
        }
        else if ((c & 0xF8) == 0xF0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            ++character_count;
            i += 3; // Skip 3 bytes.
        }
        else
        {
            // Invalid UTF8 byte.
            return -1;
        }
    }

    return character_count;
}
