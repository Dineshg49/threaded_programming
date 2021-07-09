# Concurrent Merge Sort
---

### Overview

Merge Sort is an efficient comparison-based sorting algorithm. It is a divide and conquer algorithm where we divide a problem into subproblems and when the solution to each subproblem is ready , we combine the results from the subproblems to solve the main problem.

In this problem we are given an integer an n and n numbers which we have to sort by implementing the merge sort in three different ways :
    
  - Normal Recursion
  - Child Process Recursion
  - Thread Recursion

For size of subarray < 5 , Selection Sort is performed. 

### Implementation

##### Shared Memory 
A shared memory is created so that multiple processes can access it. Here the shared memory is the array to be sorted.
Code Snipppet :
```C
key_t mem_key = IPC_PRIVATE;
int shm_id = shmget(mem_key, sizeof(int) * n, IPC_CREAT | 0666);
int* b =  (int*)shmat(shm_id, NULL, 0);

for (int i = 0; i < n; i++) {
    b[i] = arr[i];
}
```
##### Selection Sort
For size of subarray < 5 , Selection sort is performed . Selection sort finds the minimum value of the array and places it at the start of the array.
Code Snippet :
```C
void selectsort(int arr[], int low, int high)  
{  
    int i, j, min_idx;  
    for (i = low; i <= high; i++)  
    {  
        min_idx = i;  
        for (j = i+1; j <= high; j++)  
        if (arr[j] < arr[min_idx])  
            min_idx = j;    
        swap(&arr[min_idx], &arr[i]);  
    }  
}  
```

##### Normie_MergeSort 
This is the simple implementation of the merge sort without use of any child processes or threads. It divides the array to two halves and calls the same function for the two halves individually . The merge function (discussed later) merges these two sorted sub-arrays.
Code Snippet :
```C
void normie_mergesort(int arr[], int l, int r)  
{  
    if (l < r) {  
 
        int m = l + (r - l) / 2;  
   
        normie_mergesort(arr, l, m);  
        normie_mergesort(arr, m + 1, r);  
  
        merge(arr, l, m, r);  
    }  
}  
```

##### Child Recurssion Merge Sort
In this implementation two child process is created and each child process calls the same function for one of the halves. 
Code Snippet :
```C
void mergesort(int *arr, int low, int high){
     if(low<high){

          int m = low + (high - low)/2;

          if(high-low <=4)
          {
              selectsort(arr,low,high);
              return ;
          }

          int pid1=fork();
          int pid2;
          if(pid1==0){
               mergesort(arr, low, m);
               _exit(1);
          }
          else{
               pid2=fork();
               if(pid2==0){
                    mergesort(arr, m + 1, high);
                    _exit(1);
               }
               else{
                    int status;
                    waitpid(pid1, &status, 0);
                    waitpid(pid2, &status, 0);
               }
          }
          merge(arr,low,m,high);
          return;
     }
}
```
##### Threaded Merge Sort
The threaded merge sort creates separate threads for the recursion of the half subarrays of the array to be sorted.
```C
void *threaded_mergeSort(void* a){
     struct arg *args = (struct arg*) a;

     int l = args->l;
     int r = args->r;
     int *arr = args->arr;
     int m = l + (r-l)/2;
     if(r-l<=4)
     {
         selectsort(arr,l,r);
         return NULL;
     }

     struct arg a1;
     a1.l = l;
     a1.r = m;
     a1.arr = arr;
     pthread_t tid1;
     pthread_create(&tid1, NULL, threaded_mergeSort, &a1);


     struct arg a2;
     a2.l = m+1;
     a2.r = r;
     a2.arr = arr;
     pthread_t tid2;
     pthread_create(&tid2, NULL, threaded_mergeSort, &a2);

     pthread_join(tid1, NULL);
     pthread_join(tid2, NULL);

     merge(arr,l,m,r);
}
```

##### Merge Function 
The merge function merges the two sorted subarrays into a single sorted array .
```C
void merge(int arr[], int l, int m, int r)  
{  
    int i, j, k;  
    int n1 = m - l + 1;  
    int n2 = r - m;  
  
    int L[n1], R[n2];  
  

    for (i = 0; i < n1; i++)  
        L[i] = arr[l + i];  
    for (j = 0; j < n2; j++)  
        R[j] = arr[m + 1 + j];  
  

    i = 0;  
    j = 0;   
    k = l;   
    while (i < n1 && j < n2) {  
        if (L[i] <= R[j]) {  
            arr[k] = L[i];  
            i++;  
        }  
        else {  
            arr[k] = R[j];  
            j++;  
        }  
        k++;  
    }  
  
    while (i < n1) {  
        arr[k] = L[i];  
        i++;  
        k++;  
    }  
  
    while (j < n2) {  
        arr[k] = R[j];  
        j++;  
        k++;  
    }  
} 
```
### Results

The Time is noted for the different value of n and the results are as follows:

| N | Normie_Merge(s) | Child Recursion(s) | Threaded Recursion(s) | 
| ------ | ------ | ------ | ------ | 
| 10 | 0.000015 |0.000826|0.000850 |
| 100 | 0.000043 |0.006368|0.002166 |
| 1000 |0.00028 |0.026807|0.013792 |
| 10000 | 0.002329 |0.242874| 0.84304|

For n = 100 ->
 Normal Merge sort was 
 - 148.601031  times faster than concurrent_mergesort
 - 50.537309  times faster than threaded_concurrent_mergesort
#### Conclusion 
So we can conclude that The Normal Merge Sort is faster than the Merge Sorts implemented by Child Processes and threads . 




