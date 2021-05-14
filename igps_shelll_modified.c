#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define clear() printf("\033[H\033[J")

sem_t lock;

void init_shell()
{
    clear();
    printf("******************"
           "************************");
    printf("\n\n\t Welcome ");
    printf("\n\n\n*******************"
           "***********************");
    char *username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s \n", cwd);
}

void print_time()
{
    time_t now;
    time(&now);
    printf(" Date and time: %s", ctime(&now));
}

void openHelp()
{
    printf("\n***HELP LIST***"
           "\n>hello"
           "\n>change directory"
           "\n>helplist"
           "\n>showdate"
           "\n>exit"
           "\n");
    return;
}

void execute(char command[])
{
    system(command);
}

void changeDirectory()
{
    char *str = readline(">>> ");
    char *t;
    t = strtok(str, " ");
    t = strtok(NULL, " ");
    chdir(t);
}

// function to immitate cp command
int copyFile(char *argv[])
{
    int src; //source file
    int dst; //destination file
    int n;   // tracking variable
    char buf[1];

    src = open(argv[1], O_RDONLY);
    if (src == -1)
    {
        perror("can't find the source file");
        return 1;
    }

    dst = creat(argv[2], 777);
    dst = open(argv[2], O_WRONLY);
    if (dst == -1)
    {
        perror("can't create or open destination file");
        return 1;
    }

    while ((n = read(src, buf, 1)) > 0)
    {
        write(dst, buf, 1);
    }
    close(src);
    close(dst);

    return 1;
}

void calculator()
{
    char operator;
    int var;
    double first, second;
    int boolean = 1;
    while (boolean == 1)
    {
        printf("Compute: ");
        scanf("%lf %s %lf", &first, &operator, & second);

        switch (operator)
        {
        case '+':
            printf("%.2lf + %.2lf = %.2lf", first, second, first + second);
            break;
        case '-':
            printf("%.2lf - %.2lf = %.2lf", first, second, first - second);
            break;
        case '*':
            printf("%.2lf * %.2lf = %.2lf", first, second, first * second);
            break;
        case '/':
            printf("%.2lf / %.2lf = %.2lf", first, second, first / second);
            break;
        default:
            printf("Error! operator is not correct");
        }
        printf("\n Do you want to continue prees 1 or 0 for stop: ");
        scanf("%d", &var);
        boolean = var;
    }
}

void *ownCmdHandler(void *command)
{
    char *str = (void *)command;
    int NoOfOwnCmds = 8, i, j, switchOwnArg = 0;
    char *ListOfOwnCmds[NoOfOwnCmds];

    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "helplist";
    ListOfOwnCmds[2] = "hello";
    ListOfOwnCmds[3] = "showdate";
    ListOfOwnCmds[4] = "change directory";
    ListOfOwnCmds[5] = "calculator";
    ListOfOwnCmds[6] = "read file";
    ListOfOwnCmds[7] = "copy";

    sem_wait(&lock);

    for (i = 0; i < NoOfOwnCmds; i++)
    {
        if (strcmp(str, ListOfOwnCmds[i]) == 0)
        {
            switchOwnArg = i + 1;
            break;
        }
        else
            switchOwnArg = 10;
    }

    switch (switchOwnArg)
    {

    case 1:
        printf("\nGoodbye\n");
        sleep(1);
        clear();
        sem_post(&lock);
        exit(0);
    case 2:
        openHelp();
        sem_post(&lock);
        break;
    case 3:
        printf("\n Hello %s \n", getenv("USER"));
        sem_post(&lock);
        break;
    case 4:
        print_time();
        sem_post(&lock);
        break;
    case 5:
        changeDirectory();
        sem_post(&lock);
        break;
    case 6:
        calculator();
        sem_post(&lock);
        break;
    case 8:
        copyFile(command);
        sem_post(&lock);
        break;
    case 10:
        printf("\n");
        system(str);
        sem_post(&lock);
        break;
    default:
        sem_post(&lock);
        break;
    }
}

void input(char *str, int size)
{
    add_history(str);
    char *token;
    sem_init(&lock, 0, 1);
    pthread_t run[size];
    char *buffer[size];
    for (int i = 0; i < size; i++)
    {
        token = strsep(&str, ";");
        buffer[i] = token;
    }
    for (int i = 0; i < size; i++)
    {
        pthread_create(&run[i], NULL, ownCmdHandler, (void *)buffer[i]);
    }

    for (int i = 0; i < size; i++)
    {
        pthread_join(run[i], NULL);
    }

    sem_destroy(&lock);
}

int size(char *str)
{
    int count = 1;
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == ';')
            count++;
    }
    return count;
}
int main()
{
    init_shell();
    char *command;

    while (1)
    {
        printDir();
        command = readline("Shell>>> ");
        input(command, size(command));
        sleep(1);
    }

    return 0;
}