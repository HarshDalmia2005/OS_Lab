#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BUFFSIZE 1024

char *filename="dfa.txt";
int s,n;
int **delta;
int *isFinal;

int (*fd)[2];
int dupin,dupout;
pid_t *ppid;

void state_loop(int q, int *nodes, int i);

void read_file(){
    int fd;
    fd=open(filename,O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Unable to open input file in read mode...\n");
        exit(1);
    } else {
        fprintf(stderr, "New file descriptor obtained = %d\n", fd);
    }

    int save_stdin=dup(0);

    dup2(fd,0);
    close(fd);

    
    scanf("%d\n%d\n",&s,&n);
    delta=(int **)malloc(n*sizeof(int *));
    isFinal=(int *)malloc(n*sizeof(int));

    for(int i=0;i<n;i++){
        delta[i]=(int *)malloc(s*sizeof(int));
        int idx;char f;
        scanf("%d %c",&idx,&f);
        isFinal[idx]=(f=='F')?1:0;
        for(int j=0;j<s;j++){
            scanf("%d",&delta[idx][j]);
        }
    }

    dup2(save_stdin,0);
    close(save_stdin);
}

void setup_pipes(){
    fd=(int (*)[2])malloc((n+1)*sizeof(int[2]));
    for(int i=0;i<=n;i++){
        if(pipe(fd[i])<0){
            perror("pipe");
            exit(1);
        }
    }

    dupin=dup(0);dupout=dup(1);
}

void fork_process(){
    ppid=(pid_t *)malloc(n*sizeof(pid_t));
    for(int i=0;i<n;i++){
        ppid[i]=fork();
        if(ppid[i]<0){
            printf("Process Creation Failed!!");
            fflush(stdout);
            exit(1);
        }
        if(ppid[i]==0){
            signal(SIGINT, SIG_IGN);
            printf("+++ %s state %d created\n",isFinal[i]?"Final":"Non-Final",i);
            fflush(stdout);

            close(0);dup(fd[i][0]);
            int st;
            scanf("%d|",&st);

            int nodes[s];
            for(int i=0;i<s;i++){
                scanf("%d ",&nodes[i]);
            }

            state_loop(st,nodes,i);
            exit(0);
        }
    }
    sleep(1);
    printf("+++ Coordinator: %d state processes are created\n",n);
    fflush(stdout);

    for(int i=0;i<n;i++){
        char line[BUFFSIZE];
        int offset=sprintf(line,"%d|",isFinal[i]);
        for(int j=0;j<s;j++){signal(SIGINT,endUserloo
            offset+=sprintf(line+offset,"%d ",delta[i][j]);
        }
        close(1);dup(fd[i][1]);printf("%s\n",line);
        fflush(stdout);
        close(1);dup(dupout);
    }
}

void endUserloop(int sig){
    printf("\n+++ Coordinator going to terminate all processes\n");
    fflush(stdout);
    for(int i=0;i<n;i++){
        fflush(stdout);
        close(1);dup(fd[i][1]);printf("QUIT\n");fflush(stdout);close(1);dup(dupout);
        waitpid(ppid[i],NULL,0);
    }

    printf("+++ Coordinator: Bye\n");
    fflush(stdout);
    exit(0);
}

void user_loop(){
    printf("+++ Coordinator: Going to user loop\n");
    fflush(stdout);
    signal(SIGINT,endUserloop);
    while(1){
        char str[BUFFSIZE];
        printf("Enter next string: ");fflush(stdout);
        close(0);dup(dupin);scanf("%s",str);

        close(1);dup(fd[0][1]);printf("TRANSITION\n");
        fflush(stdout);
        close(1);dup(dupout);

        int m=strlen(str);
        str[m]='#';
        for(int i=0;i<=m;i++){
            int st;
            close(0);dup(fd[n][0]);
            scanf("%d",&st);
            while(getchar()!='\n');
            close(0);dup(dupin);

            close(1);dup(fd[st][1]);
            if(str[i]!='#')printf("%c\n",str[i]);
            else printf("#\n");
            fflush(stdout);
            close(1);dup(dupout);

            if(str[i]<'a' || str[i]>= 'a'+s){
                break;
            }
        }
        sleep(1);
    }
}

void state_loop(int q, int *nodes,int i){
    while(1){
        char str[BUFFSIZE];
        close(0);dup(fd[i][0]);
        scanf("%s",str);

        if(strcmp(str,"QUIT")==0){
            close(1);dup(dupout);printf("+++ State %d going to quit\n",i);
            fflush(stdout);
            exit(0);
        }
        
        if(strcmp(str,"TRANSITION") == 0){
            close(1);dup(fd[n][1]);
            printf("%d\n",i);
            fflush(stdout);
            close(1);dup(dupout);

            if(i==0)printf("0 ");

            char ch;
            close(0);dup(fd[i][0]);
            scanf(" %c",&ch);
            close(0);dup(dupin);

            int st=ch-'a';

            if(ch=='#'){
                close(1);dup(dupout);printf("%s\n",q==1?" ACCEPT":" REJECT");
            }else if(st>=0 && st<s){ 
                int next_q=delta[i][st];
                printf(" -- %c --> %d",ch,next_q);
                fflush(stdout);

                close(1);dup(fd[next_q][1]);
                printf("TRANSITION\n");
                fflush(stdout);
                close(1);dup(dupout);
            }else{
                close(1);dup(dupout);printf("Invalid Input %c\n",ch);
                fflush(stdout);
            }

        }
    }
}

int main(int argc, char *argv[]){
    if(argc>1){
        filename=argv[1];
    }

    read_file();
    setup_pipes();
    fork_process();
    user_loop();
}