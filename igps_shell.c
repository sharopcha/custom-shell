#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

// Greeting shell during startup
void init_shell()
{
    clear();
    showLogo();
    // char *username = getenv("USER");
    // printf("\n\n\nUSER is: @%s", username);
    // printf("\n");
}

// Function to take input
int takeInput(char *str)
{
    char *buf;

    buf = readline("\n>>> ");
    if (strlen(buf) != 0)
    {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    }
    else
    {
        return 1;
    }
}

// Function to print Current Directory.
void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s", cwd);
}

// Function where the system command is executed
void execArgs(char **parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1)
    {
        printf("\nFailed forking child..");
        return;
    }
    else if (pid == 0)
    {
        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nCould not execute command..");
        }
        exit(0);
    }
    else
    {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char **parsed, char **parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0)
    {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0)
    {
        printf("\nCould not fork");
        return;
    }

    if (p1 == 0)
    {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    }
    else
    {
        // Parent executing
        p2 = fork();

        if (p2 < 0)
        {
            printf("\nCould not fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0)
        {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0)
            {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        }
        else
        {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// Help command builtin
void openHelp()
{
    puts("\n*WELCOME TO MY SHELL HELP*"
         "\nCopyright @ Suprotik Dey"
         "\n-Use the shell at your own risk..."
         "\nList of Commands supported:"
         "\n>cd"
         "\n>ls"
         "\n>copy"
         "\n>cat"
         "\n>move"
         "\n>remove"
         "\n>calculator"
         "\n>permutate"
         "\n>time"
         "\n>piramide"
         "\n>palindrome"
         "\n>exit"
         "\n>clear\tclears current terminal view"
         "\n>all other general commands available in UNIX shell"
         "\n>pipe handling"
         "\n>improper space handling");

    return;
}

// Clear the terminal
void clearTerminal()
{
    clear();
}

// Commands created by ourselves
void calculator()
{
    char operator;
    double first, second;
    printf("Enter an operator (+, -, *,): ");
    scanf("%c", &operator);
    printf("Enter two operands: ");
    scanf("%lf %lf", &first, &second);

    switch (operator)
    {
    case '+':
        printf("%.1lf + %.1lf = %.1lf", first, second, first + second);
        break;
    case '-':
        printf("%.1lf - %.1lf = %.1lf", first, second, first - second);
        break;
    case '*':
        printf("%.1lf * %.1lf = %.1lf", first, second, first * second);
        break;
    case '/':
        printf("%.1lf / %.1lf = %.1lf", first, second, first / second);
        break;
        // operator doesn't match any case constant
    default:
        printf("Error! operator is not correct");
    }
}

void swap(char *x, char *y)
{
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void permute(char *a, int l, int r)
{
    int i;
    if (l == r)
        printf("%s\n", a);
    else
    {
        for (i = l; i <= r; i++)
        {
            swap((a + l), (a + i));
            permute(a, l + 1, r);
            swap((a + l), (a + i)); // backtrack
        }
    }
}

void permutate()
{
    printf("Please enter a word to permutate it:");
    char str[20];
    scanf("%s", str);
    int a = strlen(str);
    permute(str, 0, a - 1);
}

void piramide()
{
    int i, j, spc, rows, k;
    printf("Input number of rows : ");
    scanf("%d", &rows);
    spc = rows + 4 - 1;
    for (i = 1; i <= rows; i++)
    {
        for (k = spc; k >= 1; k--)
        {
            printf(" ");
        }

        for (j = 1; j <= i; j++)
            printf("* ");
        printf("\n");
        spc--;
    }
}

void palindrome()
{
    int n, reversedN = 0, remainder, originalN;
    printf("Enter an integer: ");
    scanf("%d", &n);
    originalN = n;

    // reversed integer is stored in reversedN
    while (n != 0)
    {
        remainder = n % 10;
        reversedN = reversedN * 10 + remainder;
        n /= 10;
    }

    // palindrome if orignalN and reversedN are equal
    if (originalN == reversedN)
        printf("%d is a palindrome.", originalN);
    else
        printf("%d is not a palindrome.", originalN);
}

int showTime()
{
    time_t cur_time;
    char *cur_t_string;
    cur_time = time(NULL);
    if (cur_time == ((time_t)-1))
    {
        (void)fprintf(stderr, "Failure to get the current time.\n");
        return 1;
    }
    cur_t_string = ctime(&cur_time); //convert to local time format
    if (cur_t_string == NULL)
    {
        (void)fprintf(stderr, "Failure to convert the current time.\n");
        return 1;
    }
    (void)printf("\n The Current time is : %s \n", cur_t_string);
    return 1;
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

// function to immitate cat command
int readWrite(char *argv[])
{

    int src; //source file
    int n;   // tracking variable
    char buf[1];

    src = open(argv[1], O_RDONLY);
    if (src == -1)
    {
        perror("can't find the source file");
        return 1;
    }

    while ((n = read(src, buf, 1)) > 0)
    {
        write(1, buf, 1);
    }
    close(src);
    return 1;
}

// function to immitate list command
int list(void)
{

    struct dirent *contents;
    DIR *folder = opendir(".");

    if (folder == NULL)
    {
        printf("cannot open the folder");
        return 1;
    }
    while ((contents = readdir(folder)) != NULL)
    {
        printf("%s\n", contents->d_name);
    }

    closedir(folder);
    return 1;
}

// function to immitate mv command
int move(char *argv[])
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

    dst = creat(argv[2], 0777);
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
    remove(argv[1]);
    close(dst);
    return 1;
}

// function to immitate rm command
int removeFile(char *argv[])
{
    if (argv[1] == "--help")
    {
        printf("\nusage: rm FileTodelete\n");
    }
    int status;
    status = remove(argv[1]);
    if (status == 0)
    {
        printf("successfull removed\n");
    }
    else
    {
        printf("The file is not removed\n");
    }
}

// Function to execute builtin commands
int ownCmdHandler(char **parsed)
{
    int NoOfOwnCmds = 15, i, switchOwnArg = 0;
    char *ListOfOwnCmds[NoOfOwnCmds];
    char *username;

    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "calculator";
    ListOfOwnCmds[5] = "permutate";
    ListOfOwnCmds[6] = "showTime";
    ListOfOwnCmds[7] = "piramide";
    ListOfOwnCmds[8] = "palindrome";
    ListOfOwnCmds[9] = "cat";
    ListOfOwnCmds[10] = "ls";
    ListOfOwnCmds[11] = "copy";
    ListOfOwnCmds[12] = "move";
    ListOfOwnCmds[13] = "clear";
    ListOfOwnCmds[14] = "remove";

    for (i = 0; i < NoOfOwnCmds; i++)
    {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0)
        {
            switchOwnArg = i + 1;
            break;
        }
    }

    switch (switchOwnArg)
    {
    case 1:
        printf("\nGoodbye\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        openHelp();
        return 1;
    case 4:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
               "not a place to play around."
               "\nUse help to know more..\n",
               username);
        return 1;
    case 5:
        calculator();
        return 1;
    case 6:
        // permutate();
        return 1;
    case 7:
        showTime();
        return 1;
    case 8:
        piramide();
        return 1;
    case 9:
        palindrome();
        return 1;
    case 10:
        readWrite(parsed);
        return 1;

    case 11:
        list();
        return 1;
    case 12:
        copyFile(parsed);
        return 1;
    case 13:
        move(parsed);
        return 1;
    case 14:
        clearTerminal();
        return 1;
    case 15:
        removeFile(parsed);
        return 1;

    default:
        break;
    }

    return 0;
}

// function for finding pipe
int parsePipe(char *str, char **strpiped)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else
    {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char *str, char **parsed)
{
    int i;

    for (i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char *str, char **parsed, char **parsedpipe)
{

    char *strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped);

    if (piped)
    {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
    }
    else
    {

        parseSpace(str, parsed);
    }

    if (ownCmdHandler(parsed))
        return 0;
    else
        return 1 + piped;
}

int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char *parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();

    while (1)
    {
        // print shell line
        printDir();
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString,
                                 parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}

void showLogo()
{
    printf("\n");
    printf("  _________.__                         _____.__    .___  .___.__        \n");
    printf(" /   _____/|  |__ _____ _______  _____/ ____\\__| __| _/__| _/|__| ____  \n");
    printf(" \\_____  \\ |  |  \\__  \\_  __ \\/  _ \\   __\\|  |/ __ |/ __ | |  |/    \\ \n");
    printf(" /        \\|   Y  \\/ __ \\|  | \\(  <_> )  |  |  / /_/ / /_/ | |  |   |  \n");
    printf("/_______  /|___|  (____  /__|   \\____/|__|  |__\\____ \\____ | |__|___|  /\n");
    printf("        \\/      \\/     \\/                           \\/    \\/         \\/ \n");
    printf("  ________      ___.         .__       .__  .__                         \n");
    printf(" /  _____/_____ \\_ |_________|__| ____ |  | |  | _____                  \n");
    printf("/   \\  ___\\__  \\ | __ \\_  __ \\  |/ __ \\|  | |  | \\__  \\                \n");
    printf("\\    \\_\\  \\/ __ \\| \\_\\ \\  | \\/  \\  ___/|  |_|  |__/ __ \\_              \n");
    printf(" \\______  (____  /___  /__|  |__|\\___  >____/____(____  /              \n");
    printf("        \\/     \\/    \\/              \\/               \\/               \n");
    printf("   _____ ___.          .__   _____              .__                    \n");
    printf("  /  _  \\_ |__   ____ |  |_/ ____\\____  _______|  |                   \n");
    printf(" /  /_\\  \\| __ \\ /  _ \\|  |\\   __\\__  \\ \\___   /  |                   \n");
    printf("/    |    \\ \\_\\ (  <_> )  |_|  |   / __ \\_/    /|  |__                \n");
    printf("\\____|__  /___  /\\____/|____/__|  (____  /_____ \\____/                \n");
    printf("        \\/    \\/                       \\/      \\/                     \n");
    printf(".___                      __                                         \n");
    printf("|   | ____   ____  __ ___/  |_\n");
    printf("|   |/  _ \\ /    \\|  |  \\   __\\ \n");
    printf("|   (  <_> )   |  \\  |  /|  |   \n");
    printf("|___|\\____/|___|  /____/ |__|   \n");
    printf("                \\/              \n");
}
