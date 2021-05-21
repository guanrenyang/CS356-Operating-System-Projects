import java.util.Arrays;
import java.util.Scanner;
import java.util.concurrent.*;

public class MergeSort extends RecursiveAction {
	private int begin;
	private int end;
	private Integer[] array;

	public MergeSort(int begin, int end, Integer[] array) {
		this.begin = begin;
		this.end = end;
		this.array = array;
	}

	protected void compute() {
		//Bubble sort
		if (end - begin + 1 <= 10) {
			for (int i = end; i >= begin + 1; -- i)
				for (int j = begin; j < i; ++ j)
					if (array[j].compareTo(array[j + 1]) > 0) {
						Integer temp = array[j];
						array[j] = array[j + 1];
						array[j + 1] = temp;
					}
		} else {
			int mid = begin + (end - begin) / 2;
            
			MergeSort leftTask = new MergeSort(begin, mid, array);
			MergeSort rightTask = new MergeSort(mid + 1, end, array);

			leftTask.fork();
			rightTask.fork();

			leftTask.join();
			rightTask.join();

			Integer[] temp = new Integer [end - begin + 1];
			
			int pos1 = begin, pos2 = mid + 1, k = 0;
			while (pos1 <= mid && pos2 <= end) {
				if (array[pos1].compareTo(array[pos2]) <= 0) temp[k ++] = array[pos1 ++];
				else temp[k ++] = array[pos2 ++];
			}
			while (pos1 <= mid) temp[k ++] = array[pos1 ++];
			while (pos2 <= end) temp[k ++] = array[pos2 ++];

			for (int i = 0; i < k; ++ i)
				array[i + begin] = temp[i];
		}
	}

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		Scanner sc = new Scanner(System.in);

		System.out.println("Please input the number of elements:");
		int n=sc.nextInt();
		Integer[] array = new Integer[n];
		for (int i = 0; i < n; ++ i)
		{
			array[i]=sc.nextInt();
		}

		MergeSort task = new MergeSort(0, n-1, array);

		pool.invoke(task);

		System.out.println("Array after sort:");
		System.out.println(Arrays.toString(array));
	}
}
