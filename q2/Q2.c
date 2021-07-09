#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

int min(int a , int b)
{
    if(a<b)
        return a;
    return b;
}
int n; // Companies
int m; // Vacination Zones
int o; // Students
float prob[1000]; 
int simulation;
pthread_mutex_t mutex2;
pthread_mutex_t mutex;
pthread_mutex_t mutex3;
pthread_mutex_t mutex4;

int zones_signals[1000];
int zones_vaccines[1000];
int companies_batches[1000];
int company_used[1000];

int students_waiting;
int zone_current_batches[1000];
int zone_current_company[1000];

int student_vaccine_cnt[1000];
int student_test_passed[100];
int student_test_done[1000];
int student_signal[1000];
int student_waiting[1000];
int student_allocated_zone[1000];

int total_exited;

typedef struct s{
    int id;
} s;

void* vaccineprep(void* inp)
{
    while(simulation)
    {
        
        s* inputs = (s*)inp;
        int t = (rand() %3) + 2;
        int r = (rand() %5) + 1;
       // int p = (rand() %11) + 10;

        printf(ANSI_COLOR_MAGENTA"Company %d is preparing %d batches of vaccines taking time %d seconds with prob %0.2f \n"ANSI_COLOR_RESET ,inputs->id,r,t,prob[inputs->id]);
        sleep(t);
        companies_batches[inputs->id] = r;
        printf(ANSI_COLOR_MAGENTA"Company %d has prepared the vaccines\n"ANSI_COLOR_RESET,inputs->id);

        int i =1;
        while(companies_batches[inputs->id])
        {
            pthread_mutex_lock(&mutex);
                if(zones_signals[i]==1)
                {
                    printf(ANSI_COLOR_MAGENTA"Company %d has delieverd vaccines to Vaccination zone %d ,resuming vaccination now\n"ANSI_COLOR_RESET,inputs->id,i);
                    int p = (rand() %11) + 10;
                    company_used[inputs->id]+=p;
                    companies_batches[inputs->id]--;
                    zone_current_batches[i] = p;
                    zone_current_company[i] = inputs->id;
                    zones_signals[i]=0;
                }
            pthread_mutex_unlock(&mutex);
            i++;
            if(i==m+1)
                i=1;
            if(simulation==0)
            {
               // printf("company id %d ki thread return hongyi \n",inputs->id);
                return NULL;
            }
        }
        printf(ANSI_COLOR_MAGENTA"Company %d has distributed all its batches, waiting to get them exhausted\n"ANSI_COLOR_RESET,inputs->id);
        while(company_used[inputs->id])
        {
            if(simulation==0)
                return NULL;
        }
        printf(ANSI_COLOR_MAGENTA"All the vaccines prepared by company %d are emptied . Resuming Production Now\n"ANSI_COLOR_RESET,inputs->id);
    }

}

void* zonekafunc(void* inp)
{
s* inputs = (s*)inp;
   while(simulation)
   {
    while(zones_signals[inputs->id]==1)
    {
        if(simulation==0)
        {
            return NULL;
        }
    }
    int id = inputs->id;
    printf(ANSI_COLOR_CYAN"Zone %d has recieved %d vaccines from company %d \n"ANSI_COLOR_RESET,id,zone_current_batches[id],zone_current_company[id]);
    while(zone_current_batches[id] && simulation)
    {
        pthread_mutex_lock(&mutex3);
        int f=0;
        int k;
        int arr[100];
        if(students_waiting>0)
        {
            f=1;
            pthread_mutex_lock(&mutex4);
            int ma = min(8,min(students_waiting,zone_current_batches[id]));
            if(ma==0)
                k=0;
            else
                k = (rand() % ma) +1 ;
            int cn=0;
            int i=1;
            printf(ANSI_COLOR_CYAN"Vaccination zone %d is ready to vaccinate with %d slots \n"ANSI_COLOR_RESET,id,k);
            while(cn!=k)
            {
                if(student_waiting[i]==1)
                {
                    cn++;
                    student_allocated_zone[i] = id;
                    printf(ANSI_COLOR_YELLOW "Students %d is allocated to zone %d , waiting to be vaccinated\n"ANSI_COLOR_RESET,i,id);
                    student_signal[i] = 0;
                    arr[cn] = i;
                    student_waiting[i]=0;
                }
                i++;
            }
            students_waiting-=k;
            zone_current_batches[id]-=k;
            pthread_mutex_unlock(&mutex4);
        }
        pthread_mutex_unlock(&mutex3);
        if(f)
        {
            printf(ANSI_COLOR_CYAN"Vaccination zone %d entering Vaccination Phase\n"ANSI_COLOR_RESET,id);
            for(int i=1;i<=k;i++)
            {
                printf(ANSI_COLOR_YELLOW"Student %d on vaccination zone %d is being vaccinated which has success prob of %0.2f\n"ANSI_COLOR_RESET, arr[i] , id, prob[zone_current_company[id]]);
                sleep(1);
                
                //printf("Student with id %d is checking for antibodies\n",arr[i]);
                int value = (int)(prob[zone_current_company[id]]*100);
                int rad = (rand()%100) +1 ;
                if(rad <= value)
                {
                    printf(ANSI_COLOR_GREEN"Student %d has tested positive for antibodies.\n"ANSI_COLOR_RESET,arr[i]);
                    student_test_passed[arr[i]]=1;
                }
                else
                {
                    printf(ANSI_COLOR_RED"Student %d has tested negative for antibodies. \n"ANSI_COLOR_RESET,arr[i]);
                }
                student_test_done[arr[i]]=1;
                
            }
            printf(ANSI_COLOR_CYAN"Vacination done in zone %d\n"ANSI_COLOR_RESET,id);
            company_used[zone_current_company[id]]-=k;

            if(zone_current_batches[id]==0)
            {
                printf(ANSI_COLOR_CYAN"Zone %d has run out of vaccines\n"ANSI_COLOR_RESET,id);
            }
            sleep(2);
        }
    }
    zones_signals[id] = 1;
   }
  //  printf("yeh thread retrun ho gyi %d \n",inputs->id);
   return NULL;
   // while(1);

}

void* stud(void* inp)
{
    while(simulation)
    {

        s*inputs = (s*)inp;
        int id = inputs->id;
        student_vaccine_cnt[id]++;
        if(student_vaccine_cnt[id]==4)
        {
            printf(ANSI_COLOR_RED"Student %d has failed the antibody test 3 times, Sending Home  .\n"ANSI_COLOR_RESET,id);
            total_exited++;
            if(total_exited==o)
            {
                simulation=0;
            }
            return NULL;

        }
        printf(ANSI_COLOR_YELLOW"Students %d has arrived %d time\n"ANSI_COLOR_RESET,id , student_vaccine_cnt[id]);
        pthread_mutex_lock(&mutex4);
        student_waiting[id] =1 ;
        students_waiting++;
        pthread_mutex_unlock(&mutex4);
        printf(ANSI_COLOR_YELLOW"Student %d is waiting to be allocated a Vaccination Zone \n"ANSI_COLOR_RESET,id);

        while(student_waiting[id]);

        //printf("Student %d assigned a slot on the vaccination zone %d and waiting to be vaccinated\n",id,student_allocated_zone[id]);
       // printf("students_test_done[%d] = %d\n",id,student_test_done[id]);
        while(!student_test_done[id]);
        if(student_test_passed[id])
        {
            //printf("Student with id %d has been vaccinated successfully also kitneva student %d \n", id,total_exited);
            total_exited++;
            if(total_exited==o)
            {
                simulation=0;
            }
            //printf("students ki %d thread return hogayi \n",id);
            return NULL;
        }
        student_test_done[id] = 0;

    }

}
int main()
{
    simulation =1 ;
  //  printf("Kitne Students hai ? ");
    scanf("%d",&n);
   // printf("Kitni Companies hai ? ");
    scanf("%d",&m);
   // printf("Kitne Vaccination Zones ? ");
    scanf("%d",&o);
   // printf("Enter the Probabilities of companies (Please round off till second digit place :) \n");
    for(int i =1;i<=n;i++)
    {
        scanf("%f",&prob[i]);
    }
    if(n == 0 || m==0 || o ==0)
    {
        printf("The values cannot be equal to zero \n");
        return 0;
    }
    pthread_t students_thread[o+1];
    pthread_t companies_thread[n+1];
    pthread_t zones_thread[m+1];

    s *students[o+1];
    s *companies[n+1];
    s *zones[m+1];

    for(int i=1;i<=o;i++)
    {
        students[i] = (s*)malloc(sizeof(s));
        students[i]->id = i; 
        student_signal[i] = 1;
    }
    for(int i=1;i<=n;i++)
    {
        companies[i] = (s*)malloc(sizeof(s));
        companies[i]->id = i;
    }
    for(int i=1;i<=m;i++)
    {
        zones[i] = (s*)malloc(sizeof(s));
        zones[i]->id = i;
        zones_signals[i]=1;
    }
    pthread_mutex_init(&mutex  ,NULL);
    pthread_mutex_init(&mutex2 ,NULL);
    pthread_mutex_init(&mutex3 ,NULL);
    pthread_mutex_init(&mutex4 ,NULL);
    

    for(int i=1;i<=n;i++)
    {
        pthread_create(&companies_thread[i],NULL,vaccineprep, (void*)(companies[i]));
    }

    for(int i=1;i<=m;i++)
    {
        pthread_create(&zones_thread[i],NULL,zonekafunc, (void*)(zones[i]));
    }

    for(int i=1;i<=o;i++)
        pthread_create(&students_thread[i],NULL,stud, (void*)(students[i]));

    for(int i=1;i<=n;i++)
       pthread_join(companies_thread[i],NULL);
    for(int i=1;i<=m;i++)
       pthread_join(zones_thread[i],NULL);
    for(int i=1;i<=o;i++)
       pthread_join(students_thread[i],NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&mutex4);

    printf("SIMULATION OVER ! \n");
	return 0;
}
