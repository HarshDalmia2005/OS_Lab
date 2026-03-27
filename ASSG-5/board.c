#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <sys/shm.h>
#include <signal.h>

int players,fd;
int *board, *player;

void printboard(){
    char c='A';
    for(int i=0;i<players;i++){
        if(player[i]==100)printf("%c ",c);
        c++;
    }
    printf("\n");

    printf("+---+---+---+---+---+---+---+---+---+---+\n");
    for(int i=100;i>=1;i--){
        if(i%10==0)printf("| ");
        if(i>=10)printf("%d| ",i);
        else printf(" %d| ",i);
        if(i%10==1){
            for(int j=i;j<i+10;j++){
                if(board[j]>0){
                    printf("L(%d -> %d)",j,j+board[j]);
                }
                else if(board[j]<0){
                    printf("S(%d -> %d)",j,j+board[j]);
                }
            }
            printf("\n");
            printf("+---+---+---+---+---+---+---+---+---+---+\n");
        }
    }

    c='A';
    for(int i=0;i<players;i++){
        if(player[i]==0)printf("%c ",c);
        c++;
    }
    printf("\n");
}


void handler1(int sig){
    printboard();
    write(fd,"ack",sizeof("ack"));
}

void handler2(int sig){
    shmdt(board);
    shmdt(player);
    exit(0);
}

int main(int argc,char* argv[]){
    setbuf(stdout, NULL);
    if(argc>=3){
        players=atoi(argv[1]);
        fd=atoi(argv[2]);
    }else{
        perror("Arguments missing!!");
        exit(1);
    }

    // printf("%d %d\n",players,fd);
    pid_t mypid=getpid();
    write(fd,&mypid,sizeof(pid_t));
    sleep(1);

    board=(int *)shmat(atoi(argv[3]),0,0);
    player=(int *)shmat(atoi(argv[4]),0,0);

    printboard();
    char s[20];
    sprintf(s,"Complete");
    write(fd,s,sizeof(s));

    signal(SIGUSR1,handler1);
    signal(SIGUSR2,handler2);

    while(1){
        pause();
    }

    return 0;
}