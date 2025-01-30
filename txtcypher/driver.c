#include "Cypher.h"
#include "Decypher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Please enter enough arguments");
        exit(0);
    }
    FILE *toread = fopen(argv[2], "r");
    char line[256];
    if (strcmp(argv[1], "cypher") == 0)
    {
        printf("Entered cypher mode");
        // FILE *toread = fopen("./tocypher.txt", "r");
        FILE *towrite = fopen("cyphered.txt", "w");
        // char line[256];
        while (fgets(line, sizeof(line), toread))
        {
            char *str = cypher(line, sizeof(line), 1);
            fprintf(towrite, str);
            free(str);
        }
        fclose(toread);
        fclose(towrite);
        remove(argv[2]);
    }
    if (strcmp(argv[1], "decypher") == 0)
    {
        printf("Entered decypher mode");
        // FILE *toread = fopen("cyphered.txt", "r");
        FILE *towrite = fopen("decyphered.txt", "w");
        // char line[256];
        while (fgets(line, sizeof(line), toread))
        {
            char *str = decypher(line, sizeof(line), 1);
            fprintf(towrite, str);
            free(str);
        }
        fclose(toread);
        fclose(towrite);
        remove(argv[2]);
    }
}