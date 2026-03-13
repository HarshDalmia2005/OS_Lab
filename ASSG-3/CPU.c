#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxq 1024


int n;

struct process_info{
    int id;
    int arrival_time;
    int cpu_bursts;
    int io_bursts;
    int cpu_burst_times[12]; //in ms
    int io_burst_times[12]; // in ms
    int state;
};


//ready queue implementation
struct ready_queue{
    int data[maxq];
    int front;
    int rear;
    int size;
};

void init(struct ready_queue *q){
    q->front=0;
    q->rear=0;
    q->size=0;
}

int front(struct ready_queue *q){
    if(q->size==0)return -1;
    return q->data[q->front];
}

void enqueue(struct ready_queue *q, int val){
    if(q->size==maxq){
        return;
    }
    q->rear=(q->rear+1)%maxq;
    q->data[q->rear]=val;
    q->size++;
}

void dequeue(struct ready_queue *q){
    if(q->size==0){
        return;
    }
    q->front=(q->front+1)%maxq;
    q->size--;
}
////////////////////////////////




// min priority queue
struct MinPQ{
    int idx[maxq];
    int key[maxq];
    int size;
};

void initPQ(struct MinPQ *q){
    q->size=0;
}

void pq_push(struct MinPQ *pq, int idx, int pr){
    int i=pq->size++;
    pq.idx[i]=idx;
    pq.key[i]=pr;

    while(i>0){
        int par=(i-1)/2;
        if(pq->key[par]<=pq.key[i])break;

        int tmp;
        tmp = pq->key[parent]; pq->key[parent] = pq->key[i]; pq->key[i] = tmp;
        tmp = pq->idx[parent]; pq->idx[parent] = pq->idx[i]; pq->idx[i] = tmp;

        i = parent;
    }
}

int pq_pop(struct MinPQ *pq, int *index) {
    if (pq->size == 0)
        return 0;

    *index = pq->idx[0];
    pq->size--;
    pq->idx[0] = pq->idx[pq->size];
    pq->key[0] = pq->key[pq->size];

    int i = 0;
    while (1) {
        int l = 2*i + 1;
        int r = 2*i + 2;
        int smallest = i;

        if (l < pq->size && pq->key[l] < pq->key[smallest])
            smallest = l;
        if (r < pq->size && pq->key[r] < pq->key[smallest])
            smallest = r;

        if (smallest == i) break;

        int tmp;
        tmp = pq->key[i]; pq->key[i] = pq->key[smallest]; pq->key[smallest] = tmp;
        tmp = pq->idx[i]; pq->idx[i] = pq->idx[smallest]; pq->idx[smallest] = tmp;

        i = smallest;
    }
    return 1;
}

int pq_peek(struct MinPQ *pq, int *index) {
    if (pq->size == 0)
        return 0;
    *index = pq->idx[0];
    return 1;
}
////////////////////////////////



void print_process(struct process_info p) {
    printf("Process ID: %d\n", p.id);
    printf("Arrival Time: %d ms\n", p.arrival_time);
    fflush(stdout);

    printf("CPU Bursts (%d): ", p.cpu_bursts);
    for (int i = 0; i < p.cpu_bursts; i++) {
        printf("%d", p.cpu_burst_times[i]);
        if (i <= p.cpu_bursts - 1) printf(", ");
        fflush(stdout);
    }
    printf("\n");

    printf("IO Bursts (%d): ", p.io_bursts);
    for (int i = 0; i < p.io_bursts; i++) {
        printf("%d", p.io_burst_times[i]);
        if (i <= p.io_bursts - 1) printf(", ");
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

struct process_info *readInput(){
    FILE *fp;
    fp=fopen("proc.txt","r");

    char line[1024];
    fgets(line,1024,fp);
    sscanf(line,"%d",&n);

    struct process_info *process;
    process=(struct process_info*)malloc((n+1)*sizeof(struct process_info));

    for(int i=1;i<=n;i++){
        int cpu_idx=0,io_idx=0;
        int val,offset;

        fgets(line,1024,fp);
        sscanf(line, "%d %d%n",
        &process[i].id,
        &process[i].arrival_time,
        &offset);

        char *p = line + offset;
        int is_cpu = 1;

        while (sscanf(p, "%d%n", &val, &offset) == 1) {
            if (val == -1) break;

            if (is_cpu)
                process[i].cpu_burst_times[cpu_idx++] = val;
            else
                process[i].io_burst_times[io_idx++] = val;

            is_cpu = !is_cpu;
            p += offset;
        }

        process[i].cpu_bursts=cpu_idx;
        process[i].io_bursts=io_idx;
    }
    return process;
}

void simulate(struct process_info *p){
    struct MinPQ pq;
    initPQ(&pq);

    for(int i=1;i<=n;i++){
        pq_push(pq,i,p[i].arrival_time);
        p[i].state=0;
    }

    int curr=0;
    int idx;
    pq_pop(pq,&idx);
    int s=p[idx].state;
    if(s==0){
        curr=p[idx].arrival_time;
    }else if(s%2){
        curr=p[idx].cpu_burst_times[s/2];
    }else{
        curr=p[idx].io_burst_times[s/2];
    }
    p[idx].state++;
}

int main(){

    struct process_info *Process;
    Process=readInput();
    simulate(Process);

    
}