#include <stdio.h>
#include <stdlib.h>
#include "cypher.h"
char *cypher(char *str, int size, int key)
{
    char *output = (char *)malloc((size + 1) * sizeof(char));
    for (int i = 0; i < size - 1; i++)
    {
        if (str[i] == '\n' || str[i] == '\r' || str[i] == '\0')
        {
            output[i] = '\n';
            output[i + 1] = '\0';
            break;
        }
        char value = str[i] + key + i % 3;

        output[i] = value;
    }
    return output;
}