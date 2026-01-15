#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen("foodep.txt", "r");

    int n;
    char line[256];
    fgets(line, sizeof(line), fp);
    n = atoi(line);

    printf("n=%d\n", n);
    for (int i = 0; i < n; i++)
    {
        int u;
        fgets(line, sizeof(line), fp);

        char *p = line;
        sscanf(p, "%d", &u);
        p = strchr(p, ':');
        if (p)
            p += 2;

        printf("%d: ", u);

        while (sscanf(p, "%d", &u) == 1)
        {
            printf("%d ", u);
            while (*p && *p != ' ')
                p++;
            if (*p)
                p++;
        }
        printf("\n");
    }
}