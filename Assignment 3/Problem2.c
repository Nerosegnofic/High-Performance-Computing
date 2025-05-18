#include <omp.h>
#include <stdio.h>
#include <math.h>

int main()
{
    omp_set_num_threads(4);

    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int length = sizeof(arr) / sizeof(arr[0]);
    int sum =0;
    double mean =0,variance =0, standardDeviation =0;
    // serial code

    #pragma omp parallel for reduction(+:sum)
    for(int i = 0 ; i<length; i++){
        sum+=arr[i];
    }
    mean = (double)sum/length;

    double temp =0, temp1 =0, temp2 =0;
    #pragma omp parallel for private(temp1, temp2)
    for(int i = 0 ; i<length; i++){
        temp += ((arr[i] - mean) * (arr[i] - mean));
    }

    variance = temp/length;


    standardDeviation = sqrt(variance);
    printf("standardDeviation of the array: %f\n ", standardDeviation);
}
