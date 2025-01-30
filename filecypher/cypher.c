#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void crypt(char *filename, char key, int state)
{
    FILE *toEncrypt = fopen(filename, "rb");
    FILE *encrypted;
    if (state == 0)
        encrypted = fopen("encrypted.pdf", "wb");
    else
        encrypted = fopen("decrypted.pdf", "wb");
    if (toEncrypt == NULL)
    {
        perror("Well shit\n");
    }
    fseek(toEncrypt, 0, SEEK_END);
    long long fileSize = ftell(toEncrypt);
    printf("%d\n", fileSize);
    fseek(toEncrypt, 0, SEEK_SET);
    char *buffer;
    buffer = malloc(fileSize);
    fread(buffer, sizeof(char), fileSize / sizeof(char), toEncrypt);
    for (long long i = 0; i < fileSize / sizeof(char); i++)
    {
        if (state == 0)
            buffer[i] += key;
        else
            buffer[i] -= key;
    }
    fwrite(buffer, fileSize, 1, encrypted);
    fclose(toEncrypt);
    fclose(encrypted);
    free(buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        exit(1);
    int state = 0;
    char *initial = "0";
    if (strcmp(argv[2], initial) == 0)

        state = 0;

    else
        state = 1;
    crypt(argv[1], 23, state);
}