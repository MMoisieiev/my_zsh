#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include <dirent.h>


// echo, cd, setenv, unsetenv, env, xt, pwd and which

char **setParsingValues()
{
    char **values = malloc(sizeof(char) * 8);

    for (int i = 0; i < 8; i++)
    {
        values[i] = (char *)malloc(sizeof(char) * 8);
    }
    values[0] = "echo";
    values[1] = "cd";
    values[2] = "setenv";
    values[3] = "unsetenv";
    values[4] = "env";
    values[5] = "exit";
    values[6] = "pwd";
    values[7] = "which";
    values[8] = NULL;

    return values;
}

int my_putchar(char c)
{
    return write(1, &c, 1);
}

int my_puts(char *s)
{
    int i = 0;

    while (s[i])
    {
        my_putchar(s[i]);
        i++;
    }
    return i;
}
void freeStringArray(char **stringArray)
{
    if (stringArray == NULL)
    {
        return;
    }

    int count = 0;
    while (stringArray[count] != NULL)
    {
        count++;
    }

    for (int i = 0; i < count; i++)
    {
        if (stringArray[i] != NULL)
        {
            free(stringArray[i]);
            stringArray[i] = NULL;
        }
    }

    free(stringArray);
    stringArray = NULL;
}

char **reAlloc(char **myArray)
{
    int count = 0;
    while (myArray[count] != NULL)
    {
        count++;
    }

    char **newArray = malloc(sizeof(char *) * (count + 1));

    for (int i = 0; myArray[i] != NULL; i++)
    {
        newArray[i] = malloc(sizeof(char) * (strlen(myArray[i]) + 1));
        strcpy(newArray[i], myArray[i]);
    }

    newArray[count] = NULL;
    freeStringArray(myArray);

    return newArray;
}

char **mySplit(char *input, char delimiter)
{
    int index = 0;
    char **stringArray = malloc(sizeof(char *) * 100); // allocate space for 10 pointers to char
    int wordCount = 0;
    int len = strlen(input);

    for (int i = 0; i < len; i++)
    {
        if (index == 0)
        {
            stringArray[wordCount] = malloc(sizeof(char) * len); // allocate space for a string of length 'len'
        }

        if (input[i] != delimiter)
        {
            stringArray[wordCount][index] = input[i];
            index++;
        }
        else
        {
            stringArray[wordCount][index] = '\0';
            wordCount++;
            index = 0;
        }
    }

    stringArray[wordCount][index] = '\0';
    wordCount++;
    stringArray[wordCount] = NULL;

    char **res = reAlloc(stringArray);

    return res;
}


char* whichImp(char **env, char **userInput, int m)
{
    DIR *dir;
    struct dirent *entry;
    char **line;
    int i = 0;

    while (1)
    {
        line = mySplit(env[i], '=');
        if (strcmp(line[0], "PATH") == 0)
        {
            break;
        }

        freeStringArray(line);
        i++;
    }

    char **pathBank = mySplit(line[1], ';');
    char *temp;
    int found = 0;
    char* res;
    for (int i = 0; pathBank[i] != NULL; i++)
    {

        for (int j = 0; j < 2; j++)
        {
            // Open the directory
            dir = opendir(pathBank[i]);
            if (dir == NULL)
            {
                //printf("directory didnt open %s\n", pathBank[i]);
                // Directory opening failed, continue to the next iteration
                continue;
            }

            // Store the user input

            // Read directory entries
            while ((entry = readdir(dir)) != NULL)
            {
                temp = malloc(sizeof(char) * strlen(userInput[m]));
                strcpy(temp, userInput[m]);
                if (j > 0)
                {
                    // If j is greater than 0, it means it's the second iteration, so modify the search term
                    free(temp);
                    temp = NULL;
                    temp = malloc(sizeof(char) * strlen(userInput[m]) + 5);
                    strcpy(temp, userInput[m]);
                    strcat(temp, ".exe");
                }

                // Print the search term and the current entry name
                // printf("%s - %s - %s\n", temp, entry->d_name, pathBank[i]);

                // Compare the search term with the current entry name
                if (strcmp(temp, entry->d_name) == 0)
                {
                    // Match found, print the path and file name
                    //printf("%s\\", pathBank[i]);
                    //printf("%s\n", entry->d_name);
                    res = (char*)malloc(sizeof(char) * (strlen(pathBank[i])+ strlen(temp))+2);
                    strcpy(res,pathBank[i]);
                    strcat(res,"\\");
                    strcat(res,temp);

                    found++;
                    break;
                }
                free(temp);
                temp = NULL;

                if (found > 0)
                {
                    break;
                }
            }

            // Close the directory
            closedir(dir);
        }
        if (found > 0)
        {
            break;
        }
    }
    
    
    
    if(found > 0){
        return res;
    }
    else{
        perror("error");
        return NULL;
    }
    
}
/*int execMD(char** env, char** userInput) {
    char* path;
    if ((path = whichImp(env, userInput, 0)) == NULL) {
        return -1;
    }

    char** args = userInput;  // The arguments to be passed to the executable, including the command itself
    args[0] = path;  // Set the command as the path to the executable

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        return -1;
    } else if (pid == 0) {
        // Child process
        execve(path, args, env);
        perror("Execve failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        } else {
            // Child process did not terminate normally
            return -1;
        }
    }
}
*/






char *getFirstWord(char *input)
{

    int i = 0;
    char *command = (char *)malloc(sizeof(char) * 20);

    while (input[i] != ' ' || input[i] != '\0')
    {
        command[i] = input[i];
        i++;
    }
    command[i] = '\0';

    return command;
}

int isBuiltin(char *command, char **bltn)
{

    int i = 0;

    while (bltn[i] != NULL)
    {

        if (strcmp(bltn[i], command) == 0)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void runBuiltins(int bct, char **userInput, char **env)
{
    char cwd[PATH_MAX];
    int i = 0;
    char* res;
    switch (bct)
    {

    case 0:
        for (int i = 1; userInput[i]; i++)
        {
            my_puts(userInput[i]);
            my_putchar(' ');
        }

        break;

    case 1:
        // cd
        if (chdir(userInput[1]) != 0)
        {
            perror("no such directory");
        }
        // chdir(userInput[2]);
        break;

    case 2:
        // setenv
        break;

    case 3:
        // unsetenv
        break;

    case 4:
        while (env[i])
        {
            puts(env[i]);
            // putchar('\n');
            i++;
        }
        break;

    case 5:

        exit(0);
        break;

    case 6:
        // pwd

        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            puts(cwd);
        }
        else
        {
            perror("there was an error getting your path");
            exit(0);
        }
        break;

    case 7:
        res = whichImp(env, userInput,1);
        puts(res);
        free(res);
        res = NULL;
        // which
        break;
    }
    // echo, cd, setenv, unsetenv, env, xt, pwd and which
}
char *removeBackN(char *line)
{

    for (int i = 0; line[i]; i++)
    {
        if (line[i] == '\n')
        {
            line[i] = '\0';
        }
    }
    return line;
}
char *removeSpace(char *input)
{
    int i = strlen(input);

    if (input[i - 2] == ' ')
    {
        input[i - 2] = '\0';
    }
    return input;
}

int main(int ac, char **av, char **env)
{

    (void)ac;
    (void)av;

    // exit(0);
    int fd = 0;
    int bct = 0; // builtins count
    char *userInput;
    // int i = 0;
    char **builtins = setParsingValues();

    while (1)
    {
        printf("GTN - zsh ");
        userInput = (char *)malloc(sizeof(char) * 20);

        userInput = my_readline(fd);

        userInput = removeSpace(userInput);
        userInput = removeBackN(userInput);

        char **line = mySplit(userInput, ' ');

        bct = isBuiltin(line[0], builtins);

        if (bct >= 0)
        {
            runBuiltins(bct, line, env);
        }

        free(userInput);
        userInput = NULL;
        freeStringArray(line);
        printf("\n");
    }

    return 0;
}
