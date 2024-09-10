//copy form https://blog.csdn.net/jenie/article/details/106741221
#include "header/bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _Bits {
    char *bits;
    unsigned int length;
};

bits bit_new(unsigned int length)
{
    bits new_bits = (bits)malloc(sizeof(struct _Bits));
    if (new_bits == NULL)
        return NULL;

    int char_nums = sizeof(char) * (length >> 3) + 1;
    new_bits->bits = (char *)malloc(char_nums);
    if (new_bits == NULL) {
        free(new_bits);
        return NULL;
    }
    memset(new_bits->bits, 0, char_nums);
    new_bits->length = length;

    return new_bits;
}

void bit_destroy(bits bit)
{
    free(bit->bits);
    free(bit);
}

unsigned int bit_length(bits bit)
{
    return bit->length;
}

void bit_set(bits bit, unsigned int pos, unsigned char value)
{
    unsigned char mask = 0x80 >> (pos & 0x7);
    if (value) {
        bit->bits[pos>>3] |= mask;
    } else {
        bit->bits[pos>>3] &= ~mask;
    }
}

char bit_get(bits bit, unsigned int pos)
{
    unsigned char mask = 0x80 >> (pos & 0x7);
    return (mask & bit->bits[pos>>3]) == mask ? 1 : 0;
}

