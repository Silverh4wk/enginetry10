#if !defined(DATASTRUCTURES_H)
#include "Helpers.h"
#include <cstdlib>
#include <cstdio>
#include <list>
//Node
typedef struct Node
{
  int Data;
  Node* Next ;
  Node* Prev;
  Node(int val) : Data(val), Next(nullptr), Prev(nullptr) {}
}Node;



class LinkedList{
public:

  Node* NewNode;

    // Container pointers and counter
    Node* Head;   // Pointer to the first node (when this is acting as container)
    Node* Tail;   // Pointer to the last node (when this is acting as container)
    int count;

    // Constructors/Destructor and assignment operators
  //   LinkedList();
  LinkedList() : Head(nullptr), Tail(nullptr), count(0) {}
  //LinkedList(int data);
  //  LinkedList(const LinkedList& other);
  //  LinkedList& operator=(const LinkedList& other);
  //  LinkedList(LinkedList&& other);
  //  LinkedList& operator=(LinkedList&& other);
  // ~LinkedList();

    // Methods for list manipulation
  //  void insertNodeAtIndex(int index, int data);
    void push_front(int Data);
  // void push_back(int Data);
  // void pop_front();
  // void deleteNodeAtIndex(int index);
  // Node getNode(int index);
  // void deleteNode(Node * NewNode);

    // Queue operations
    //void enqueue(int Data);
    //void dequeue();
    //int front();
    //int back();

    //void clearList();
    void forwardPrint();
  // voi2d reversePrint();
  // int getSize();
  // bool empty();
};

// Push front: add a node to the front of the list.
 void LinkedList::push_front(int Data) {
   Node* temp = new Node(Data);
    if (Head == nullptr) {
        Head = Tail = temp;
    }
    else {
        temp->Next = Head;
        Head->Prev = temp;
        Head = temp;
    }
    count++;
}


void LinkedList::forwardPrint() {
Node* temp = Head;
while (temp != nullptr) {
    printf("%d ", temp->Data);
    temp = temp->Next;
}
     printf("\n");
}


 Node* NodeForward(Node* HeadNode, Node* NewNode) {
    if (NewNode == NULL)
        return HeadNode;

    NewNode->Next = NULL;

    if (HeadNode == NULL) {
        return NewNode;
    }

    Node* temp = HeadNode;
    while (temp->Next != NULL) {
        temp = temp->Next;
    }

    temp->Next = NewNode;
    return HeadNode;
}


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
           printf("after swapping sorting: ");
for (int i = 0; i < ArraySize; i++) {
    
    printf("%d, ", A[i]);
}
 printf("\n");

        }

    }
}

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
Heapify(T A [] , int HeapSize, T ParentIndex)
{
  int MaxIndex = ParentIndex;

  int LeftIndex  = 2*ParentIndex +1;
  int RightIndex = 2*ParentIndex +2;

  if (LeftIndex < HeapSize && A[LeftIndex] > A[MaxIndex])
        MaxIndex = LeftIndex;
    if (RightIndex < HeapSize && A[RightIndex] > A[MaxIndex])
        MaxIndex = RightIndex;  
    if (MaxIndex != ParentIndex) {
      Swap(A+ParentIndex,A+MaxIndex);
        Heapify (A, HeapSize, MaxIndex);
      }
    }
//heapsize = 4, parent index is 0, A [10,2,30,1]
//maxindex = 0 = [10]
//left index = 0*2+1= 1[2]
//Right index = 0*2 + 2 = 2[30]
//first if ... 2 < 10 so we dont do anything there
//second if , 30 >10 so we set the maxindex to the right child hence its bigger
//if the max index doesnt equal to the parent index we just simply swap them HEY THATS EASY WHAT IS THIS   

  //HeapSort
  template<typename T>
  internal void
  HeapSort( T A[],int ArraySize)
  {
    for (int ParentIndex = (ArraySize-2)/2; ParentIndex>=0; ParentIndex --)
    {
      Heapify(A,ArraySize,ParentIndex);
    }
    for (int i = 0; i < ArraySize; i++) {
    
    printf("%d, ", A[i]);
}
 printf("\n");

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

template<typename T>
internal void
Merge(T Array[],int LeftIndex, int MidPoint,int RightIndex)
{
  //populating the left list and the right list
   std::list<T> LeftList={};
  for (int i = 0; i < MidPoint-LeftIndex+1; i++)
      LeftList.push_back(Array[LeftIndex + i]);
  
  std::list<T> RightList={};
  for (int i = 0; i < RightIndex-MidPoint; i++)
      RightList.push_back(Array[MidPoint+1+i]);

  int k = LeftIndex;
  while (!LeftList.empty() && !RightList.empty()) {
      if (LeftList.front() < RightList.front())
      {
          Array[k++] = LeftList.front();
          LeftList.pop_front();
      }
      else
      {
          Array[k++] = RightList.front();
          RightList.pop_front();
      }
  }
  while (!LeftList.empty())
  {
      Array[k++] = LeftList.front();
      LeftList.pop_front();
  }
  
  while (!RightList.empty())
  {
      Array[k++] = RightList.front();
      RightList.pop_front();
  }

  }

  
  
template <typename T>
internal void
MergeSort(T Array[], int LeftIndex,int RightIndex)
{
  int MidPoint = 0;
  //continue if it has more than one element to be sorted
  if (LeftIndex < RightIndex)
  {
    //Get the mid point, then sort each segement
    MidPoint = (LeftIndex + RightIndex)/2;
    MergeSort(Array, LeftIndex,MidPoint);
    MergeSort(Array, MidPoint+1, RightIndex);
    //finally merge them all together
    Merge(Array,LeftIndex,MidPoint,RightIndex);
  }
    
}

template <typename T>
internal void 
Partition(std::list<T>& List,
    std::list<T>& Less,
    std::list<T>& Equal,
    std::list<T>& Greater)
{
    if (List.empty()) return;

    T pivot = List.front();

    while (!List.empty()) {
        T e = List.front();
        List.pop_front();  

        if (e < pivot) Less.push_back(e);
        else if (e > pivot) Greater.push_back(e);
        else Equal.push_back(e);
    }
}
//using stl only temporary
template <typename T>
internal void 
QuickSort(std::list<T>& List) 
{
    if (List.size() <= 1) return;

    std::list<T> Less, Equal, Greater;

    Partition(List, Less, Equal, Greater);

    QuickSort(Less);
    QuickSort(Greater);

    List.clear();  

    while (!Less.empty()) {
        List.push_back(Less.front());
        Less.pop_front();
    }

    while (!Equal.empty()) {
        List.push_back(Equal.front());
        Equal.pop_front();
    }

    while (!Greater.empty()) {
        List.push_back(Greater.front());
        Greater.pop_front();
    }
}

#define DATASTRUCTURES_H
#endif

