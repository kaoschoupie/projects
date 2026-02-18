#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(size_t length, size_t i, char c)
{
    return ((i + c) % length);
}
void crypt(char *filename, char *key, int state)
{
    FILE *toEncrypt = fopen(filename, "rb");
    if (toEncrypt == NULL)
    {
        perror("Well shit\n");
    }
    // get file size
    fseek(toEncrypt, 0, SEEK_END);
    long long fileSize = ftell(toEncrypt);
    fseek(toEncrypt, 0, SEEK_SET);
    // create buffer to store the same amount of data as the file
    char *buffer;
    buffer = malloc(fileSize);
    // store file in buffer
    fread(buffer, sizeof(char), fileSize / sizeof(char), toEncrypt);
    size_t length = strlen(key);
    // encrypt or decrypt the buffer
    for (long long i = 0; i < fileSize / sizeof(char); i++)
    {
        if (state == 0)
            buffer[i] += key[hash(length, i, key[i % length])];
        else
            buffer[i] -= key[hash(length, i, key[i % length])];
    }
    // close file and delete file
    fclose(toEncrypt);
    if (remove(filename) != 0)
    {
        printf("Fatal failure");
        exit(1);
    }
    // create new file with same name as original
    // this is done to avoid permission denied issues
    toEncrypt = fopen(filename, "wb");
    fwrite(buffer, fileSize, 1, toEncrypt);
    free(buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        exit(1);
    int state = 0;
    char *initial = "0";
    if (strcmp(argv[3], initial) == 0)
    {
        state = 0;
    }
    else
    {
        state = 1;
    }
    crypt(argv[1], argv[2], state);
}