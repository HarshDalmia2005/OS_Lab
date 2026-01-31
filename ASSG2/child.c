#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int n;
pid_t *cno;
int ballThrown=0,out=0;
int curr=1;


void readPIDS(){
    FILE *fp;
    fp=fopen("childpid.txt","r");

    fscanf(fp,"%d",&n);
    cno=(int *)malloc((n+1) * sizeof(pid_t));

    for(int i=1;i<=n;i++){
        fscanf(fp,"%d",&cno[i]);
        if(getpid()==cno[i])curr=i;
    }
    fclose(fp);
}

void ball(int sig){
    if(sig==SIGUSR2){
        // printf("Ball is thrown to %d",getpid());
        fflush(stdout);
        double prob=(double)rand()/RAND_MAX;

        if(prob<=0.2){
            ballThrown=1;
            out=1;
            kill(getppid(),SIGUSR2);
        }else{
            ballThrown=2;
            kill(getppid(),SIGUSR1);
        }
    }else{
        if(ballThrown==1){
            ballThrown=0;
            printf("MISS    ");
        }else if(ballThrown==2){
            ballThrown=0;
            printf("CATCH   ");
        }else{
            if(!out)printf("....    ");
            else printf("       ");
        }

        fflush(stdout);
        if(curr+1<=n)kill(cno[curr+1],SIGUSR1);
        else{
            FILE *fp=fopen("dummypid.txt","r");
            pid_t dpid;
            fscanf(fp,"%d",&dpid);
            fclose(fp);
            
            kill(dpid,SIGINT);
        }
    }
    
}

void endgame(){
    if(!out)printf("\n+++ Child %d: Yay! I am the winner!\n",curr);
    fflush(stdout);
    exit(0);
}
int main(int argc, char *argv[]){
    usleep(1000000);
    readPIDS();

    srand(getpid());
    
    signal(SIGINT,endgame);
    signal(SIGUSR1,ball);
    signal(SIGUSR2,ball);

    while(1) pause();
    exit(0);
}