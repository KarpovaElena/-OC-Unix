#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void bubbleSort(int* arr, int size);
void insertSort(int* a, int size);
void selectSort(int* arr, int size);
pid_t exec_in_fork(int* arr, int size, void (*sfunc)(int*, int));


int main() {
pid_t buble_sort_pid, insert_sort_pid, select_sort_pid;
int status;

srand(time(NULL));
// инициализация генератора случайных чисел

int N  = 100000;
int ra[N];
//printf("Массив случайных чисел\n");
 for (int i = 0; i < N; i++) {
        ra[i] = rand() % (N + 1);
        //printf("%d ", ra[i]);
    }
/*  printf("\n");
printf("Пузырек\n");
bubbleSort(ra, N);
printf("\n");
printf("Вставка\n");
insertSort(ra, N);
printf("\n");
printf("Выборка\n");
selectSort(ra, N);
printf("\n"); */

    srand(time(0));
    buble_sort_pid = exec_in_fork(ra, N, bubbleSort);
    insert_sort_pid = exec_in_fork(ra, N, insertSort);
    select_sort_pid = exec_in_fork(ra, N, selectSort);
    
    for (int i=0; i<3; ++i) {
        wait(&status);
    }
return 0;
}

void bubbleSort(int* arr, int size)
{
    int tmp, i, j;
    
    for(i = 0; i < (size - 1); ++i) // i - номер прохода
    {            
        for(j = 0; j < (size - 1); ++j) // внутренний цикл прохода
        {     
            if (arr[j+1] < arr[j]) 
            {
                tmp = arr[j + 1]; 
                arr[j + 1] = arr[j]; 
                arr[j] = tmp;
            }
        }
    }
    /*for(i = 0; i < size; i++) 
    {            
      printf("%d ", arr[i]);  
    }*/
}

void insertSort(int* a, int size) 
{
    int i, j, tmp;
    for (i = 1; i < size; ++i) // цикл проходов, i - номер прохода
    {
        tmp = a[i]; 
        for (j = i - 1; j >= 0 && a[j] > tmp; --j) // поиск места элемента в готовой последовательности 
            a[j + 1] = a[j];    // сдвигаем элемент направо, пока не дошли
        a[j + 1] = tmp; // место найдено, вставить элемент    
    }
    /*for(i = 0; i < size; i++) 
    {            
      printf("%d ", a[i]);  
    }*/
}

void selectSort(int* arr, int size) 
{
    int tmp, i, j, pos;
    for(i = 0; i < size; ++i) // i - номер текущего шага
    { 
        pos = i; 
        tmp = arr[i];
        for(j = i + 1; j < size; ++j) // цикл выбора наименьшего элемента
        {
            if (arr[j] < tmp) 
            {
               pos = j; 
               tmp = arr[j]; 
            }
        }
        arr[pos] = arr[i]; 
        arr[i] = tmp; // меняем местами наименьший с a[i]
       // printf("%d ", arr[i]);
    }
}

pid_t exec_in_fork(int* arr, int size, void (*sfunc)(int*, int)){
    pid_t pid;
    clock_t time_start, time_stop;
    double exec_time;
    
    pid = fork();
    if (pid == 0){
        printf("%d started\n", getpid());
        time_start = clock();
        sfunc(arr, size);
        time_stop = clock();
        exec_time = (double)(time_stop - time_start) / CLOCKS_PER_SEC;
        printf("%d: %f seconds\n", getpid(), exec_time);
        _exit(0);
    }
    return pid;
}
