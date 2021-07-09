#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<semaphore.h> 
#include<string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int min(int a , int b)
{
    if(a<b)
        return a;
    return b;
}
int k; // Total performers
int a,e; // acoustic and electrical stages
int c; // coordinators
int t1,t2; // minimum and maximum time for performance
int t; //maximum wait time
int simulation =1;

pthread_mutex_t mutex;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
sem_t sem;
sem_t sem2;
sem_t dope[100];
sem_t cord;

pthread_mutex_t sing_mutex;
pthread_mutex_t sing_mutex2;
sem_t sing_sem;
sem_t sing_sem2;
sem_t sing_dope[100];

int total_exited;

typedef struct s{
    int id;
    char name[25];
    char ins[1];
    int tim;
    int stage;
} s;

typedef struct s1{
    int id;
}s1;

s *per[1000];

int per_status[100];
int ac_stage_status[100];
int ac_stage_status_singer[100];
int el_stage_status[100];
int el_stage_stautus_singer[100];
int acc_stage_non_sing_per[100];
int acc_stage_sing_per[100];
int ele_stage_non_sing_per[100];
int ele_stage_sing_per[100];
int cn1=1,cn2;
int sign[100];

pthread_t stages[100];
pthread_t e_stages[100];
pthread_t perr[100];

void tshirt(int id)
{
  //  s1* inputs = (s1*)inp;
//    int id = inputs->id;
    sem_wait(&cord);

    printf(ANSI_COLOR_CYAN"%s is collecting T-shirt \n" ANSI_COLOR_RESET, per[id]->name);
    sleep(2);
    printf(ANSI_COLOR_CYAN"%s left after collecting T-shirt \n"ANSI_COLOR_RESET,per[id]->name);
    
    sem_post(&cord);

}

void* acous(void * inp)
{
    s1* inputs = (s1*)inp;
    int id = inputs->id;
    sem_wait(&sem);
    if(per[id]->stage==3)
    {
        if(sem_trywait(&dope[id])==-1)
        {
            sem_post(&sem);
            return NULL;
        }
    }
    if(sign[id]==1)
    {
        sem_post(&sem);
        return NULL;
    }
    per_status[id]=2;
    pthread_mutex_lock(&mutex2);
    int current_stage;
    for(int i=1;i<=a;i++)
    {
        if(ac_stage_status[i]==0)
        {
            ac_stage_status[i]=1;
            current_stage =i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex2);
    acc_stage_non_sing_per[current_stage] = id; 
    int perf_time = rand()%(t2-t1 +1 ) + t1; 
    printf(ANSI_COLOR_GREEN"%s performing %s at accoustic stage %d for %d seconds \n"ANSI_COLOR_RESET,per[id]->name, per[id]->ins,current_stage,perf_time);
    sleep(perf_time);
    int f=0;
    if(acc_stage_sing_per[current_stage]!=0)
    {
        sleep(2);
        f=1;
    }
    printf(ANSI_COLOR_GREEN"%s performance ended at accoustic stage %d \n"ANSI_COLOR_RESET,per[id]->name,current_stage);
    ac_stage_status[current_stage]=0;
    acc_stage_non_sing_per[current_stage] = 0; 
    acc_stage_sing_per[current_stage]=0;
    if(f)
    {
        ac_stage_status_singer[current_stage]=0;
        sem_post(&sing_sem);
    }
    sem_post(&sem);
    tshirt(id);
    return NULL;
}

void* singer_acous(void *inp)
{
    s1* inputs = (s1*)inp;
    int id = inputs->id;
    sem_wait(&sing_sem);
    //if(per[id]->stage==3)
   // {
        if(sem_trywait(&sing_dope[id])==-1)
        {
            sem_post(&sing_sem);
            return NULL;
        }
   // }
    if(sign[id]==1)
    {
        sem_post(&sing_sem);
        return NULL;
    }
    per_status[id]=2;
    pthread_mutex_lock(&mutex2);
    int current_stage;
    for(int i=1;i<=a;i++)
    {
        if(ac_stage_status_singer[i]==0)
        {
            if(acc_stage_non_sing_per[i]==0)
            {
                ac_stage_status[i]=1;
                sem_wait(&sem);
            }
            ac_stage_status_singer[i]=1;
            current_stage =i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex2);
    acc_stage_sing_per[current_stage] = id; 
    int perf_time = rand()%(t2-t1 +1 ) + t1; 
    if(acc_stage_non_sing_per[current_stage]==0)
        printf(ANSI_COLOR_GREEN"%s is singing on the accoustic stage %d solo for %d seconds\n"ANSI_COLOR_RESET,per[id]->name,current_stage ,perf_time);
    else
    {
         printf(ANSI_COLOR_GREEN"%s joined %s performance , performance extended by 2 seconds  \n"ANSI_COLOR_RESET,per[id]->name, per[acc_stage_non_sing_per[current_stage]]->name);
        return NULL;
    }
    
//   printf("yaha kyun nhi aa rha bkl \n");
    sleep(perf_time);
  //  printf("aa toh gya mc \n");
    printf(ANSI_COLOR_GREEN"%s has left the  accoustic stage %d \n"ANSI_COLOR_RESET,per[id]->name,current_stage);
    ac_stage_status_singer[current_stage]=0;
    ac_stage_status[current_stage]=0;
    acc_stage_sing_per[current_stage]=0;
    sem_post(&sing_sem);
    sem_post(&sem);
    return NULL;
}

void* elec(void * inp)
{
    s1* inputs = (s1*)inp;
    int id = inputs->id;

    sem_wait(&sem2);
    if(per[id]->stage==3)
    {
        if(sem_trywait(&dope[id])==-1)
        {
            sem_post(&sem2);
            return NULL;
        }
    }
    if(sign[id]==1)
    {
        sem_post(&sem2);
        return NULL;
    }
    per_status[id]=2;
    int current_stage;
    pthread_mutex_lock(&mutex);
    for(int i=1;i<=e;i++)
    {
        if(el_stage_status[i]==0)
        {
            current_stage =i;
            el_stage_status[i]=1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    ele_stage_non_sing_per[current_stage] = id;
    int perf_time = rand()%(t2-t1 +1 ) + t1; 
    printf(ANSI_COLOR_BLUE"%s performing %s on electric stage %d for %d \n"ANSI_COLOR_RESET,per[id]->name, per[id]->ins,current_stage,perf_time);
    sleep(perf_time);
    int f=0;
    if(ele_stage_sing_per[current_stage]!=0)
    {
        sleep(2);
        f=1;
       // printf("%s performance ended on electric stage %d \n",per[ele_stage_sing_per[current_stage]]->name,current_stage);
        
    }
    printf(ANSI_COLOR_BLUE"%s performance ended at electric stage %d  \n"ANSI_COLOR_RESET,per[id]->name,current_stage);
    el_stage_status[current_stage]=0;
    ele_stage_non_sing_per[current_stage] = 0;
    ele_stage_sing_per[current_stage]=0;
    sem_post(&sem2);
    if(f)
    {
        el_stage_stautus_singer[current_stage]=0;
        sem_post(&sing_sem2);
    }
    tshirt(id);
    return NULL;
}

void* singer_elec(void * inp)
{
    s1* inputs = (s1*)inp;
    int id = inputs->id;

    sem_wait(&sing_sem2);
    //if(per[id]->stage==3)
   // {
        if(sem_trywait(&sing_dope[id])==-1)
        {
            sem_post(&sing_sem2);
            return NULL;
        }
    //}
    if(sign[id]==1)
    {
        sem_post(&sing_sem);
        return NULL;
    }
    int current_stage;
    per_status[id]=2;
    pthread_mutex_lock(&mutex);
    for(int i=1;i<=e;i++)
    {
        if(el_stage_stautus_singer[i]==0)
        {
            if(ele_stage_non_sing_per[i]==0)
            {
                el_stage_status[i]=1;
                sem_wait(&sem2);
            }
            current_stage =i;
            el_stage_stautus_singer[i]=1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    ele_stage_sing_per[current_stage] = id;
    int perf_time = rand()%(t2-t1 +1 ) + t1;
    if(ele_stage_non_sing_per[current_stage]==0)
        printf(ANSI_COLOR_BLUE"%s is singing on the electic stage %d solo for %d seconds\n"ANSI_COLOR_RESET,per[id]->name,current_stage,perf_time);
    else
    {
         printf(ANSI_COLOR_BLUE"%s joined %s performance , performance extended by 2 seconds\n"ANSI_COLOR_RESET,per[id]->name,per[ele_stage_non_sing_per[current_stage]]->name);
         return NULL;
    }

    sleep(perf_time);
    
    printf(ANSI_COLOR_BLUE"%s has left the electric stage %d \n"ANSI_COLOR_RESET,per[id]->name,current_stage);
    el_stage_stautus_singer[current_stage]=0;
    el_stage_status[current_stage]=0;
    ele_stage_sing_per[current_stage]=0;
    sem_post(&sing_sem2);
    sem_post(&sem2);
    return NULL;
}


void* performer(void * inp)
{
    s1* inputs = (s1*)inp;
    int id = inputs->id;
    sleep(per[id]->tim);
    printf(ANSI_COLOR_RED"%s %s arrived \n" ANSI_COLOR_RESET, per[id]->name,per[id]->ins);
    per_status[id]=1;
    pthread_mutex_lock(&mutex3);
    if(per[id]->stage==0)
        pthread_create(&stages[cn1++],NULL,acous,(void*)per[id]);
    else if(per[id]->stage==1)
        pthread_create(&stages[cn1++],NULL,elec,(void*)per[id]);
    else if(per[id]->stage==3)
    {
        pthread_create(&stages[cn1++],NULL,elec,(void*)per[id]);
        pthread_create(&stages[cn1++],NULL,acous,(void*)per[id]);
    }
    else
    {
        pthread_create(&stages[cn1++],NULL,singer_elec,(void*)per[id]);
        pthread_create(&stages[cn1++],NULL,singer_acous,(void*)per[id]);
    }
    pthread_mutex_unlock(&mutex3);
    sleep(t);
    // sem_timed_wait();
    if(per_status[id]==1)
    {
        sign[id] = 1;
        printf(ANSI_COLOR_MAGENTA"%s %s left due to impatience \n"ANSI_COLOR_RESET,per[id]->name,per[id]->ins);  
    }
    return NULL;
}


int main()
{
    
   // printf("Total Performers : ");
    scanf("%d",&k);

   // printf("Accoustic Stages : ");
    scanf("%d",&a);

   // printf("Electric Stages :");
    scanf("%d",&e);

    //printf("Total Coordinators : ");
    scanf("%d",&c);

    //printf("Minimum and Maximum Time of Performance \n");
    scanf("%d%d",&t1,&t2);

    //printf("Angry Time :");
    scanf("%d",&t);

    //printf("Enter sbhi ki Information\n");
    for(int i=1;i<=k;i++)
    {
        per[i] = (s*)malloc(sizeof(s));
        per[i]->id = i;
        scanf("%s%s%d",per[i]->name,per[i]->ins,&per[i]->tim);
        if(strcmp(per[i]->ins,"v")==0)
            per[i]->stage =0 ; // acoustic_musician
        else if(strcmp(per[i]->ins,"b")==0)
            per[i]->stage =1; // electric
        else if(strcmp(per[i]->ins,"s")==0)
            per[i]->stage = 2; // singer
        else
            per[i]->stage = 3; // both
    }
    s1 *perfo[k+1];

    for(int i=1;i<=k;i++)
    {
        perfo[i] = (s1*)malloc(sizeof(s1));
        perfo[i]->id = i;
    }

    sem_init(&sem, 0, a);
    sem_init(&sem2, 0, e);
    sem_init(&cord,0,c);

    sem_init(&sing_sem, 0, a);
    sem_init(&sing_sem2, 0, e);

    for(int i=1;i<=k;i++)
        sem_init(&dope[i], 0, 1);
    
    pthread_mutex_init(&mutex  ,NULL);
    pthread_mutex_init(&mutex2  ,NULL);
    pthread_mutex_init(&mutex3, NULL);

    for(int i=1;i<=k;i++)
        sem_init(&sing_dope[i], 0, 1);

    pthread_mutex_init(&sing_mutex  ,NULL);
    pthread_mutex_init(&sing_mutex2  ,NULL);

    for(int i=1;i<=k;i++)
    {
        pthread_create(&perr[i],NULL,performer,(void*)perfo[i]);
    }

  //  printf("yaha aa gya \n");
    


    for(int i=1;i<=k;i++)
       pthread_join(perr[i],NULL);

   // printf("here\n");
    for(int i=1;i<cn1;i++)
       pthread_join(stages[i],NULL);
    // printf("yaha aa gya \n");

    // printf("yaha aa gya \n");
    // for(int i=0;i<=k;i++)
    //    pthread_join(e_stages[i],NULL);
    // printf("yaha aa gya \n");

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    for(int i=0;i<=k;i++)
        sem_destroy(&dope[i]);
    // printf("yaha aa gya \n");
    
    pthread_mutex_destroy(&sing_mutex);
    pthread_mutex_destroy(&sing_mutex2);
    for(int i=1;i<=k;i++)
        sem_destroy(&sing_dope[i]);
    
    sem_destroy(&sing_sem);
    sem_destroy(&sing_sem2);
    sem_destroy(&sem);
    sem_destroy(&sem2);
    sem_destroy(&cord);
    printf("Finished\n");
    return 0;
    
}
