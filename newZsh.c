#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include "stringedits.h"
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

int getOS()
{
    struct utsname info;
    int osCode;
    if (uname(&info) != -1)
    {
        if (strcmp(info.sysname, "Windows") == 0)
        {
            // Windows-specific code
            osCode = 1;
            // printf("Operating System Code: %d\n", osCode);
        }
        else
        {
            // Non-Windows code
            osCode = 0;

            // printf("Operating System Code: %d\n", osCode);
        }
    }
    else
    {
        // Error handling
        puts("Failed to determine the operating system.\n");
        exit(0);
    }

    return osCode;
}

char *whichImp(char **env, char **userInput,int m)  //  "m" for  mode (if 'which' search for 2nd word in the
                       //  userInput, else search for command starting at 0)
{
    DIR *dir;
    struct dirent *entry;
    char **line;
    int i = 0;
    // printf("check");
    char splitter;
    char *slash;
    if (getOS() == 1)
    {
        splitter = ';';
        slash = "\\";
    }
    else
    {
        splitter = ':';
        slash = "/";
    }
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

    char **pathBank = mySplit(line[1], splitter);
    char *temp;
    int found = 0;
    char *res;

    for (int i = 0; pathBank[i] != NULL; i++)
    {
        // Open the directory
        dir = opendir(pathBank[i]);
        if (dir == NULL)
        {
            // printf("directory didnt open %s\n", pathBank[i]);
            // Directory opening failed, continue to the next iteration
            continue;
        }

        // printf("check %d %d %s", i , j, pathBank[i]);
        // Read directory entries
        while ((entry = readdir(dir)) != NULL)
        {
            temp = malloc(sizeof(char) * strlen(userInput[m]));
            strcpy(temp, userInput[m]);
            if (getOS() == 1)
            {
                // If j is greater than 0, it means it's the second iteration,
                // so modify the search term
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
                // printf("%s\\", pathBank[i]);
                // printf("%s\n", entry->d_name);
                res = (char *)malloc(
                    sizeof(char) * (strlen(pathBank[i]) + strlen(temp)) + 2);
                strcpy(res, pathBank[i]);
                strcat(res, slash);
                strcat(res, temp);

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

        if (found > 0)
        {
            break;
        }
    }

    if (found > 0)
    {
        return res;
    }
    else
    {
        perror("error1");
        return NULL;
    }
}
int execMD(char **env, char **userInput)
{
    char *path;
    if ((path = whichImp(env, userInput, 0)) == NULL)
    {
        return -1;
    }

    char **args = userInput;  // The arguments to be passed to the executable,
                              // including the command itself
    args[0] = path;           // Set the command as the path to the executable

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Fork failed");
        return -1;
    }
    else if (pid == 0)
    {
        // Child process
        execve(path, args, env);
        perror("Execve failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
        else
        {
            // Child process did not terminate normally
            return -1;
        }
    }
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
    char *res;
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
            res = whichImp(env, userInput, 1);
            puts(res);
            free(res);
            res = NULL;
            // which
            break;
    }
    // echo, cd, setenv, unsetenv, env, xt, pwd and which
}

int main(int ac, char **av, char **env)
{
    (void)ac;
    (void)av;

    // exit(0);
    int fd = 0;
    int bct = 0;  // builtins count
    char *userInput;
    // int i = 0;
    char **builtins = setParsingValues();

    while (1)
    {
        puts("GTN - zsh ");
        
        userInput = (char *)malloc(sizeof(char) * 20);
        
        userInput = my_readline(fd);
        //fflush(stdin);
        userInput = removeSpace(userInput);
        userInput = removeBackN(userInput);

        char **line = mySplit(userInput, ' ');

        bct = isBuiltin(line[0], builtins);

        if (bct >= 0)
        {
            runBuiltins(bct, line, env);
        }
        else
        {
            if (whichImp(env, line, 0) != NULL)
            {
                execMD(env, line);
            }
        }

        free(userInput);
        userInput = NULL;
        freeStringArray(line);
        // putchar('\n');
    }

    return 0;
}
