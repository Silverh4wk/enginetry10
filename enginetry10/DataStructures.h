#pragma once
#include "Helpers.h"
//Node
struct Node
{
  
};

// Selection Sort
template<typename T>
internal void
SelectionSort(T A[], int ArraySize) {
    int MaxIndex = 0;
    int  temp = 0;
    for (int i = ArraySize - 1; i >= 1; i--) {
        MaxIndex = i;
        for (int j = 0; j < i; j++) {
            if (A[MaxIndex] < A[j])
                MaxIndex = j;
        }
        if (MaxIndex != i)
        {
            Swap(A + i,A+MaxIndex);
        }

    }
}// Selection Sort
template<typename T>
internal void
DescSelectionSort(T A[], int ArraySize) {
    int MinIndex = 0;
    int  temp = 0;
    for (int i = ArraySize - 1; i >= 1; i--) {
        MinIndex = i;
        for (int j = 0; j < i; j++) {
            if (A[MinIndex] > A[j])
                MinIndex = j;
        }
        if (MinIndex != i)
        {
            Swap(A + i,A+MinIndex);
        }

    }
}

template<typename T>
internal void
Heapify (T A [] , int HeapSize, T Index)
{
  int MaxIndex = Index;

  int LeftIndex  = 2*Index +1;
  int RightIndex = 2*Index +2;

  if (LeftIndex < HeapSize && A[LeftIndex] > A[MaxIndex])
      MaxIndex = LeftIndex;
  if (RightIndex < HeapSize && A[RightIndex] > A[MaxIndex])
      MaxIndex = RightIndex;  
  if (MaxIndex != Index) {
    Swap(A+Index,A+MaxIndex);
      Heapify (A, HeapSize, MaxIndex);
    }
  }
  


//HeapSort
template<typename T>
internal void
HeapSort( T A[],int ArraySize)
{
  for (int ParentIndex = (ArraySize-2)/2; ParentIndex>=0; ParentIndex --)
  {
    Heapify(A,ArraySize,ParentIndex);
  }

  for(int i = ArraySize-1; i>= 1; i--)
  {
    Swap(A, A+i);
    Heapify(A,--ArraySize,0);
  }
}

template <typename T>
internal void
DescHeapSort( T A[],int ArraySize)
{
  for (int ParentIndex = (ArraySize-2)/2; ParentIndex>=0; ParentIndex --)
  {
    Heapify(A,ArraySize,ParentIndex);
  }

  for(int i = ArraySize-1; i>= 1; i--)
  {
    Swap(A, A+i);
    Heapify(A,ArraySize,ArraySize-1);
  }
}
