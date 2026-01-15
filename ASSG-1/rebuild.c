#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *ptr = fopen("foodep.txt", "r");
    if (!ptr)
    {
        perror("Unable to open foodep.txt!");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), ptr);
    int n;
    sscanf(line, "%d", &n);
    // parent
    if (argc == 2)
    {
        char s[n + 1];
        for (int i = 0; i < n; i++)
            s[i] = '0';
        s[n] = '\0';
        FILE *fp = fopen("done.txt", "w");
        if (fp == NULL)
        {
            perror("Error in opening write file");
            return 1;
        }

        fputs(s, fp);
        fclose(fp);
    }

    int u=atoi(argv[1]), v;
    int dep[n + 1];
    while (fgets(line, sizeof(line), ptr))
    {
        int temp;
        sscanf(line, "%d:", &temp);
        if (temp == u)
            break;
    }

    char *p = line;
    p = strchr(p, ':');
    if (*p)
        p += 2;

    int i = 0;
    while (sscanf(p, "%d", &v) == 1)
    {
        dep[i++] = v;
        while (*p && *p != ' ')
            p++;
        if (*p)
            p++;
    }

    // for each dependency v of u
    for (int j = 0; j < i; j++)
    {
        int w = dep[j];
        FILE *fptr = fopen("done.txt", "r");
        if (!fptr)
        {
            perror("reading done.txt failed!!");
            exit(EXIT_FAILURE);
        }
        char vis[256];
        fgets(vis, sizeof(vis), fptr);
        fclose(fptr);
        if (vis[w - 1] == '0')
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork failed!!");
                exit(1);
            }
            else if (pid == 0)
            {
                char s[10];
                sprintf(s, "%d", w);
                execl("./rebuild", "rebuild", s, "child", (char *)NULL);
                perror("exec failed!!");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
    }

    FILE *fptr;
    char vis[256];

    fptr = fopen("done.txt", "r");
    fgets(vis, sizeof(vis), fptr);
    fclose(fptr);

    vis[u - 1] = '1';
    printf("foo%d rebuilt",u);
    if(i>0)printf(" from");
     for (int j = 0; j < i; j++)
    {
        int w = dep[j];
        printf(" foo%d",w);
        if(j!=i-1)printf(",");
    }
    printf("\n");
    fptr = fopen("done.txt", "w");
    fputs(vis, fptr);
    fclose(fptr);
    fclose(ptr);
}