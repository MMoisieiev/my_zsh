#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int READLINE_READ_SIZE = 1; // remember to give value to READLINE_READ_SIZE, this whould have been
                            // the next reason why while loop did not start
char *READLINE_STORAGE = NULL;

char *my_strchr(char *string, char needle)
{
    for (int i = 0; string[i]; i++)
    {
        if (string[i] == needle)
        {
            return &string[i + 1];
        }
    }

    return NULL;
}
char *my_strncpy(char *param_1, char *param_2, int param_3)
{
    int index = 0;
    while (index != param_3)
    {
        param_1[index] = param_2[index];

        index++;
    }
    param_1[index] = '\0';

    return param_1;
}
int my_strlen(char *strlen_char) // count a string's legnth
{
    int length = 0;
    while (strlen_char[length] != '\0')
    {
        length++;
    }
    return length;
}

char *my_strcpy(char *param_1, char *param_2)
{
    int i;
    for (i = 0; param_2[i] != '\0'; i++)
    {
        param_1[i] = param_2[i];
    }
    param_1[i] = '\0';
    return param_2;
}

char *my_strcat(char *strg1, char *strg2)
{
    if (strg1 == NULL || strg1[0] == '\0')
    {
        return strg2;
    }
    if (strg2 == NULL || strg2[0] == '\0')
    {
        return strg1;
    }
    int i = 0;
    while (strg1[i] != '\0')
    {
        i++;
    }
    int j = 0;
    while (strg2[j] != '\0')
    {
        strg1[i + j] = strg2[j];
        j++;
    }
    strg1[i + j] = '\0';
    return strg1;
}

char *my_readline(int fd)
{

    char buffer[READLINE_READ_SIZE + 1];
    char *tmp;
    char *endPointer = NULL;
    char *res;
    int bytesRead;

    char *currBuff = NULL;
    char *tmpBuff = NULL;
    char *newRes = NULL;

    if (READLINE_STORAGE)
    {
        if ((tmp = my_strchr(READLINE_STORAGE, '\n')) != NULL)
        {
            res = malloc(sizeof(char) * strlen(READLINE_STORAGE) - strlen(tmp) + 1);
            my_strncpy(res, READLINE_STORAGE, strlen(READLINE_STORAGE) - strlen(tmp));
            free(READLINE_STORAGE);
            READLINE_STORAGE = malloc(sizeof(char) * strlen(tmp) + 1);
            my_strcpy(READLINE_STORAGE, tmp);
            tmp = NULL;
            return res;
        }
        else
        {
            currBuff = malloc(sizeof(char) * strlen(READLINE_STORAGE) + 1);

            my_strcpy(currBuff, READLINE_STORAGE);
            free(READLINE_STORAGE);
            READLINE_STORAGE = NULL;
        }
    }

    while ((bytesRead = read(fd, buffer, READLINE_READ_SIZE)) > 0)
    {
        buffer[bytesRead] = '\0';
        if (buffer[0] == '\n')
        {
        }

        if (currBuff)
        { // if mainBuffer already exists
            tmpBuff = malloc(sizeof(char) * strlen(currBuff) + strlen(buffer) + 1);

            my_strcpy(tmpBuff, currBuff); // copy existing stuff
            strcat(tmpBuff, buffer);      // copy what read

            free(currBuff); // free curr buffer and set it to NULL
            currBuff = NULL;

            currBuff = malloc(sizeof(char) * strlen(tmpBuff) + 1); // reallocate and add back
            my_strcpy(currBuff, tmpBuff);
            free(tmpBuff);
        }
        else
        { // if storage doesnt exist
            currBuff = malloc(sizeof(char) * strlen(buffer) + 1);
            my_strcpy(currBuff, buffer);
        }
        if ((endPointer = my_strchr(currBuff, '\n')) != NULL)
        { // if there is a full line in buffer i return it and save leftovers
            // in the storage
            newRes = malloc(sizeof(char) * strlen(currBuff) - strlen(endPointer) + 1);

            my_strncpy(newRes, currBuff, strlen(currBuff) - strlen(endPointer));
            // printf("newRes = %s,\n", newRes);
            READLINE_STORAGE = malloc(strlen(endPointer) + 1); // malloc
            my_strcpy(READLINE_STORAGE, endPointer);

            endPointer = NULL;

            free(currBuff); // free buffer
            currBuff = NULL;
            return newRes;
        }
    }

    return currBuff;
}
