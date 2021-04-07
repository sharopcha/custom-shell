#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

int main(void)
{
    struct dirent *contents;
    DIR *folder = opendir(".");

    if (folder == NULL)
    {
        printf("cannot open the folder");
        exit(1);
    }
    while ((contents = readdir(folder)) != NULL)
    {
        printf("%s\n", contents->d_name);
    }

    closedir(folder);
    return 0;
}