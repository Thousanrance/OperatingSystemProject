#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int *array;
int *sorted_array;

typedef struct
{
	int left;
	int right;
} index;

void quicksort(int *arr, int low, int high)
{
	int i, j, pivot;
	int tmp;
	if(low < high)
	{
		pivot = low;
		i = low;
		j = high;
		while(i < j)
		{
			while(arr[i] <= arr[pivot] && i < high)
			{
				i++;
			}
			while(arr[j] > arr[pivot])
			{
				j--;
			}
			if(i < j)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
		}
		tmp = arr[pivot];
		arr[pivot] = arr[j];
		arr[j] = tmp;
		
		quicksort(arr, low, j - 1);
		quicksort(arr, j + 1, high);
	}
}

void *sort(void *ind)
{
	int left = ((index*)ind)-> left;
	int right = ((index*)ind) -> right;
	quicksort(array, left, right);
}

void *merge(void *ind)
{
	int left = ((index*)ind)-> left;
	int right = ((index*)ind) -> right;
	int mid = (left + right)/2;
	
	int smlst1 = left;
	int smlst2 = mid + 1;
	int id = 0; //for sorted array
	
	/*
	for(int i = 0; i <= right; i++)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
	*/
	
	while(id <= right)
	{
		//printf("id = %d\n", id);
		if(smlst1 <= mid && smlst2 <= right)
		{
			if(array[smlst1] <= array[smlst2])
			{
				sorted_array[id] = array[smlst1];
				id++;
				smlst1++;
			}
			else
			{
				sorted_array[id] = array[smlst2];
				id++;
				smlst2++;
			}
		}
		else if(smlst1 <= mid)
		{
			sorted_array[id] = array[smlst1];
			id++;
			smlst1++;
		}
		else
		{
			sorted_array[id] = array[smlst2];
			id++;
			smlst2++;	
		}
	}
}


int main()
{
	int size;
	printf("What is the size of your array? Please input(must bigger than 1): \n");
	scanf("%d",&size);
	
	if(size == 1)
	{
		printf("Invalid size number!\n");
		return 1;
	}
	
	array = (int*)malloc(sizeof(int)*size);
	sorted_array = (int*)malloc(sizeof(int)*size);
	
	printf("Please print your array: \n");
	for(int i = 0; i < size; i++)
	{
		scanf("%d",&array[i]);
	}
	
	//prepare index for each thread
	index *ind[3];
	for(int i = 0; i < 3; i++)
	{
		ind[i] = (index*) malloc(sizeof(index));
	}
	
	//for 2 quicksort
	ind[0] -> left = 0;
	ind[0] -> right = (size - 1)/2;
	ind[1] -> left = (size - 1)/2 + 1;
	ind[1] -> right = size - 1;
	//for merge
	ind[2] -> left = 0;
	ind[2] -> right = size - 1;
	
	//create 3 thread
	pthread_t tid1, tid2, tid3;
	pthread_attr_t attr1, attr2, attr3;
	pthread_attr_init(&attr1); //for subarray1
	pthread_attr_init(&attr2); //for subarray2
	pthread_attr_init(&attr3); //for merge
	
	pthread_create(&tid1, &attr1, sort, ind[0]);
	pthread_create(&tid2, &attr2, sort, ind[1]);
	pthread_create(&tid3, &attr3, merge, ind[2]);
	
	//run 3 thread
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
	
	//print result
	printf("The sorted array is: \n");
	for(int i = 0; i < size; i++)
	{
		printf("%d ", sorted_array[i]);
	}
	printf("\n");
	
	//free space
	free(array);
	free(sorted_array);
	for(int i = 0; i < 3; i++) free(ind[i]);
	
	return 0;
}
