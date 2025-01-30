#include <stdio.h>
#include <stdlib.h>
#include "decypher.h"
char *decypher(char *str, int size, int key)
{
    char *output = (char *)malloc((size + 1) * sizeof(char));
    for (int i = 0; i < size; i++)
    {
        if (str[i] == '\n' || str[i] == '\0')
        {
            output[i] = '\n';
            output[i + 1] = '\0';
            break;
        }
        char value = str[i] - key - i % 3;

        output[i] = value;
    }
    return output;
}