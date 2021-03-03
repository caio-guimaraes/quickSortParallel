#include <stdio.h>
#include <stdlib.h>

// Algorithm 10.5
// The sequential Algorithm of Quick Sorting
QuickSort(double A[], int i1, int i2) {
  if ( i1 < i2 ){
    double pivot = A[i1];
    int is = i1;
    for ( int i = i1+1; i<i2; i++ )
      if ( A[i] ≤ pivot ) {
          is = is + 1;
          swap(A[is], A[i]);
      }
    swap(A[i1], A[is]);
    QuickSort (A, i1, is);
    QuickSort (A, is+1, i2);
  }
}
