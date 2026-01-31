#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


int n,nextcust,left;
pid_t *childno;
char cno[16];
int *status;


pid_t *createChilds(){
    FILE *fp;

    if(n<=0){
        fprintf(stderr,"*** I need atleast one child");
        exit(1);
    }

    childno=(int *)malloc((n+1) * sizeof(pid_t));
    status=(int *)malloc((n+1) * sizeof(int));
    left=n;
    fp=(FILE *)fopen("childpid.txt","w");

    fprintf(fp,"%d\n",n);
    for(int i=1;i<=n;i++){
        status[i]=1;
        pid_t pid=fork();
        if(pid==0){
            sprintf(cno,"%d",i);
            execlp("./child","child",cno,NULL);
            fprintf(stderr,"*** Child %d is unable to exec\n",i);
            exit(1);
        }else if(pid<0){
            fprintf(stderr,"*** Error!!");
            exit(1);
        }
        childno[i]=pid;
        fprintf(fp,"%d ",pid);
    }
    fprintf(fp,"\n");
    fclose(fp);
    return childno;
}

void printStatus(){
    pid_t dummy=fork();
    if (dummy == 0) execlp("./dummy", "./dummy", NULL);
    FILE *fp=fopen("dummypid.txt","w");
    fprintf(fp,"%d",dummy);
    fclose(fp);

    kill(childno[1],SIGUSR1);
    waitpid(dummy,NULL,0);
    printf("\n");
    fflush(stdout);
}

void endgame(){
    for(int i=1;i<=n;i++){
        kill(childno[i],SIGINT);
        waitpid(childno[i],NULL,0);
    }
    exit(0);
}

void throwball(int sig){
    printStatus();

    if(sig==SIGUSR2){
        status[nextcust]=0;
        nextcust++;
        left--;
    }

    do {
        nextcust++;
        if (nextcust > n) nextcust = 1;
    } while (status[nextcust] == 0);

    if(left>1)kill(childno[nextcust],SIGUSR2);
    else{
        endgame();
    }
}

void startThrow(){
    nextcust=1;
    kill(childno[nextcust],SIGUSR2);
    while(1) pause();
}

int main(int argc, char *argv[]){
    if(argc==2){
        n=atoi(argv[1]);
    }else{
        fprintf(stderr, "*** There should be atleast one children");
        exit(1);
    }

    srand((unsigned int)getpid());
    signal(SIGUSR1,throwball);
    signal(SIGUSR2,throwball);
    

    childno=createChilds();
    printf("Parent: %d child processes created\n",n);
    printf("Parent: Waiting for child Processes to read child database\n");
    fflush(stdout);
    usleep(2000000);

    startThrow();
    exit(0);
}
