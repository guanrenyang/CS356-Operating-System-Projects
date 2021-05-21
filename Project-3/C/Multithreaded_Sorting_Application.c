# include <stdio.h>
# include <pthread.h>
# include <stdlib.h>
int *array;
int *result;
struct parameter
{
    int Start;
    int End;
};
void* runner_sort(void* para){
    struct parameter* para_sort=(struct parameter*) para;
    if(para_sort->Start>=para_sort->End)
        return NULL;
    for(int i=para_sort->End;i>para_sort->Start;--i){
        for(int j=para_sort->Start;j<i;++j){
            if(array[j]>array[j+1]){
                int tmp=array[j];
                array[j]=array[j+1];
                array[j+1]=tmp;
            }
        }
    }

}
void* runner_merge(void* para){

    struct parameter* para_merge=(struct parameter*) para;

    int n=para_merge->End+1;
    
    int end_former=n/2-1;
    int end_later=n-1;
    
    int i=0,j=end_former+1;
    int index_result=0;
    while (i<=end_former&&j<=end_later)
    {
        if(array[i]<array[j]){
            result[index_result]=array[i];
            i++;
        }
        else{
            result[index_result]=array[j];
            j++;
        }
        index_result++;
    }
    if(i<=end_former){
        for(i=i;i<=n/2-1;++i){
            result[index_result]=array[i];
            index_result++;
        }
    }
    else if(j<=end_later){
        for(j=j;j<n-1;++j){
            result[index_result]=array[j];
            index_result++;
        }
    }
    
}

int main(){
    
    printf("Please input the length of your unsorted array:");
    
    int n;
    scanf("%d",&n);

    //the unsorted array             -> array
    //the result of the sorted array -> result
    array=(int*) malloc(n*sizeof(int));
    result=(int*) malloc(n*sizeof(int));

    printf("Please input the elements to sort:\n");
    for(int i=0;i<n;++i){
        scanf("%d",&array[i]);
    }

    pthread_t tid_former;//thread for sorting the former half
    pthread_t tid_later; //thread for sorting the later half
    struct parameter* para_sort_former=(struct parameter*) malloc(sizeof(struct parameter));// prarameters for the former thread
    struct parameter* para_sort_later=(struct parameter*) malloc(sizeof(struct parameter)); // prarameters for the later thread
    
    para_sort_former->Start=0;
    para_sort_former->End=n/2-1;
    para_sort_later->Start=n/2;
    para_sort_later->End=n-1;

        int ERROR[2];
    ERROR[0]=pthread_create(&tid_former,NULL,runner_sort,para_sort_former);
    ERROR[1]=pthread_create(&tid_later,NULL,runner_sort,para_sort_later);

    pthread_join(tid_former,NULL);
    pthread_join(tid_later,NULL);
    
    pthread_t tid_merge;
        
    struct parameter* para_merge=(struct parameter*) malloc(sizeof(struct parameter));
    para_merge->Start=0;
    para_merge->End=n-1;

        int ERROR_MERGE;
    ERROR_MERGE=pthread_create(&tid_merge,NULL,runner_merge,para_merge);
        if(ERROR_MERGE){
            printf("Fail to create the merging thread!\n");
            exit(1);
        }

    pthread_join(tid_merge,NULL);

    printf("Array after sort:\n");
    for(int i=0;i<n;++i){
        printf("%d ",result[i]);
    }
printf("\n");
    return 0;
}
