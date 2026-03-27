 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

int players,fd,BP,shmid1,shmid2;
pid_t *p;
int *nextply;  // shared memory pointer
int *board,*player;
int *rank;

void game(int sig){
    char ply='A'+(*nextply);

    if(player[players]==1){
        player[*nextply]=100;
        player[players]--;
        printf("Player %c exits with rank=%d\n",ply,(*rank)++);
        kill(BP,SIGUSR1);
        exit(0);
    }

    printf("*********************************************************\n");
    printf("Player %c: ",ply);
    int tot=0,cnt=0,dice;
    do{
        if(cnt>0)printf("+ ");
        dice=rand()%6+1;
        tot+=dice;
        printf("%d ",dice);
        if(dice==6)cnt++;
        if(cnt==3){
            cnt=0;
            tot=0;
            printf("X ");
        }
    }while(dice==6);

    printf("\n");

    int nextpos=player[(*nextply)]+tot;
    int flag=0;
    while(nextpos<=100 && board[nextpos]!=0){
        if(board[nextpos]>0) printf("Ladder at cell %d, Jump to %d\n", nextpos, nextpos+board[nextpos]);
        else printf("Snake at cell %d, Jump to %d\n", nextpos, nextpos+board[nextpos]);
        
        nextpos+=board[nextpos];
        for(int i=0;i<players;i++){
            if(nextpos!=100 && player[i]==nextpos){
                flag=i+1; 
                break;
            }
        }
        if(flag){
            nextpos=player[(*nextply)];
            char occ='A'+(flag-1);
            printf("Move not permitted (cell already occupied by %c)\n",occ);
            break;
        }
    }

    if(flag==0 && nextpos<100){
        player[(*nextply)]=nextpos;
        printf("Player %c moved to %d\n",ply,nextpos);
    }else if(nextpos==100){
        player[(*nextply)]=100;
        player[players]--;
        printf("Player %c exits with rank=%d\n",ply,(*rank)++);
        kill(BP,SIGUSR1);
        exit(0);
    }else if(flag==0 && nextpos>100){
        printf("Move not permitted (cannot go beyond 100)\n");
    }

    kill(BP,SIGUSR1); 
}

void end(int sig){
    printf("Player %c exited\n",'A'+(*nextply));
    exit(0);
}

void func(){
    srand(time(NULL) ^ getpid());
    signal(SIGUSR1,game);
    signal(SIGINT,end);

    while(1){
        pause();
    }
}

void handler1(int sig){
    if(player[players] == 0) return;
    do {
        *nextply = (*nextply + 1) % players;
    } while (player[*nextply] == 100);
    kill(p[*nextply],SIGUSR1);
}

void handler2(int sig){
    for(int i=0;i<players;i++){
        kill(p[i],SIGINT);
        waitpid(p[i],NULL,0);
    }
    shmdt(board);
    shmdt(player);
    exit(0);
}
int main(int argc, char *argv[]){
    srand(time(NULL));
    if(argc>=3){
        players=atoi(argv[1]);
        fd=atoi(argv[2]);
        BP=atoi(argv[3]);
        shmid1=atoi(argv[4]);
        shmid2=atoi(argv[5]);
    }else{
        perror("Arguments missing");
        exit(1);
    }

    

    pid_t pid=getpid();
    write(fd,&pid,sizeof(pid_t));

    p=(pid_t *)malloc(players*sizeof(pid_t));
    board=(int *)shmat(shmid1,0,0);
    player=(int *)shmat(shmid2,0,0);

    int shmid3=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
    nextply=(int *)shmat(shmid3,0,0);
    *nextply=0;

    int shmid4=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
    rank=(int *)shmat(shmid4,0,0);
    *rank=1;

    for(int i=0;i<players;i++){
        p[i]=fork();
        if(p[i]==0){ //child
            func();
            exit(0);
        }
    }

    //PP
    signal(SIGUSR1,handler1);
    signal(SIGUSR2,handler2);
    while(1){
        pause();
    }
    
}