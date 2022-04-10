import java.util.concurrent.*;
import java.util.Random;

public class ForkJoinTask_Quick extends RecursiveAction
{
	static final int THRESHOLD = 10;
	static final int SIZE = 100;
	
	private int begin;
	private int end;
	private int[] array;
	
	public ForkJoinTask_Quick(int begin, int end, int[] array)
	{
		this.begin = begin;
		this.end = end;
		this.array = array;
	}
	
	protected void compute()
	{
		if(end - begin < THRESHOLD) //The size of the segment is less than THRESHOLD
		{
			this.selectionsort(begin, end);
			return;
		}
		else
		{
			int mid = quicksort(begin, end);
			ForkJoinTask_Quick leftTask = new ForkJoinTask_Quick(begin, mid, array);
			ForkJoinTask_Quick rightTask = new ForkJoinTask_Quick(mid + 1, end, array);
			
			leftTask.fork();
			rightTask.fork();
			
			leftTask.join();
			rightTask.join();
			
			this.merge(begin, mid, end);
			
			return;
		}
	}
	
	public int quicksort(int low, int high)
	{
		/*
		if(high - low < THRESHOLD) //The size of the segment is less than THRESHOLD
		{
			this.selectionsort(low, high);
			return;
		}
		*/
		
		int pivot = low;
		int i = low;
		int j = high;
		int tmp;
		while(i < j)
		{
			while(array[i] <= array[pivot] && i < high)
			{
				i++;
			}
			while(array[j] > array[pivot])
			{
				j--;
			}
			if(i < j)
			{
				tmp = array[i];
				array[i] = array[j];
				array[j] = tmp;
			}
		}
		tmp = array[pivot];
		array[pivot] = array[j];
		array[j] = tmp;
		
		//quicksort(low, j - 1);
		//quicksort(j + 1, high);	
		
		return pivot;
	}
	
	private void selectionsort(int begin, int end)
	{
		int min;
		for(int i = begin; i < end; i++)
		{
			min = array[i];
			for(int j = i + 1; j <= end; j++)
			{
				if(array[j] < min)
				{
					min = array[j];
					array[j] = array[i];
					array[i] = min;
				}
			}
		}
	}
	
	private void merge(int left, int mid, int right)
	{
		int[] sorted_array = new int[SIZE];
		
		int smlst1 = left;
		int smlst2 = mid + 1;
		int id = left;
		
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
		
		for(int i = left; i<= right; i++)
		{
			array[i] = sorted_array[i];
		}
	}
	
	public static void main(String[] args)
	{
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];
		
		Random rand = new Random(); //for create random number
		int x;
		
		System.out.println("The original array is: ");
		for(int i = 0; i < SIZE; i++)
		{
			x = rand.nextInt(SIZE);
			array[i] = x;
			
			System.out.print(array[i]);
			System.out.print(' ');
		}
		System.out.print('\n');
		
		ForkJoinTask_Quick task = new ForkJoinTask_Quick(0, SIZE-1, array);
		
		pool.invoke(task);
		
		System.out.println("The sorted array is: ");
		for(int i = 0; i < SIZE; i++)
		{
			System.out.print(array[i]);
			System.out.print(' ');
		}
		System.out.print('\n');
	}
}
