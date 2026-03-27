#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

int players;
int fd[2];
pid_t BP,PP;

void endgame(int *board, int *player){
    printf("Hit return to end the game...\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    kill(PP,SIGUSR2);
    waitpid(PP,NULL,0);
    kill(BP,SIGUSR2);
    waitpid(BP,NULL,0);
    shmdt(board);
    shmdt(player);
    exit(0);
}

void initialise_board(int *board){
    FILE *fp = fopen("ludo.txt", "r");
    for(int i=0;i<=101;i++)board[i]=0;
    for(int i=0;i<=101;i++){
        char c;int a,b;
        if(fscanf(fp," %c %d %d",&c,&a,&b)!=3)break;
        if(c=='L'){
            board[a]=b-a;
        }
        else if(c=='S'){
            board[a]=b-a;
        }else break;
    }

    // for(int i=0;i<=101;i++){
    //     printf("%d ",board[i]);
    // }
    fclose(fp);
}

void initialise_player(int *player){
    for(int i=0;i<=players;i++){
        player[i]=0;
    }
    player[players]=players;
}

void play(int *board,int *player){
    printf("playing\n");
    kill(PP,SIGUSR1);
    char ack[20];
    read(fd[0],ack,sizeof(ack));
    if(strcmp(ack,"ack")==0){
        printf("Board updated\n");
    }

    if(player[players]==0){
        endgame(board,player);
    }
}

void autoplay(int t,int *board,int *player){
 printf("Starting autoplay...\n");
    while(player[players]>0){
        usleep(t);
        play(board,player);
    }
}

int main(int argc, char *argv[]){
    
    if(argc>=2){
        players = atoi(argv[1]);
    }
    else{
        perror("Enter number of players");
        exit(1);
    }

    //creating shared memory
    int shmid1=shmget(IPC_PRIVATE, 102*sizeof(int), 0777|IPC_CREAT);
    int shmid2=shmget(IPC_PRIVATE, (players+1)*sizeof(int), 0777|IPC_CREAT);

    int *board = (int*)shmat(shmid1, NULL, 0);
    int *player = (int*)shmat(shmid2, NULL, 0);

    initialise_board(board);
    initialise_player(player);

    if (pipe(fd) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    //forking XBP AND XPP
    pid_t pid1=fork();
    
    if(pid1==0){
        char s[20],s2[20],s3[20],s4[20];
        sprintf(s,"%d",players);
        sprintf(s2,"%d",fd[1]);
        sprintf(s3,"%d",shmid1);
        sprintf(s4,"%d",shmid2);
        execlp("xterm","xterm","-T","Board","-fs","15",
            "-geometry","100x24+1000+100","-bg","#000033",
            "-e","./board",s,s2,s3,s4,NULL);
    }

    read(fd[0],&BP,sizeof(BP));

    pid_t pid2=fork();
    if(pid2==0){
        char s[20],s2[20],s3[20],s4[20],s5[20];
        sprintf(s,"%d",players);
        sprintf(s2,"%d",fd[1]);
        sprintf(s3,"%d",BP);
        sprintf(s4,"%d",shmid1);
        sprintf(s5,"%d",shmid2);
        execlp("xterm","xterm","-T","Players","-fs","15",
            "-geometry","100x24+1000+100","-bg","#000033",
            "-e","./players",s,s2,s3,s4,s5,NULL);
    }

    read(fd[0],&PP,sizeof(PP));
    char confirm[20];
    read(fd[0],confirm,sizeof(confirm));

    if(strcmp(confirm,"Complete")==0){
        printf("Board Initialised\n");
    }

    while(1){
        char cmd[20];
        scanf("%s",cmd);
        if(strcmp(cmd,"next")==0){
            play(board,player);
        }else if(strcmp(cmd,"exit")==0){
            endgame(board,player);
        }else{
            int t;
            scanf("%d",&t);
            scanf("%s",cmd);

            if(strcmp(cmd,"autoplay")==0){
                autoplay(t,board,player);
                break;
            }
        }
    }
    return 0;
}