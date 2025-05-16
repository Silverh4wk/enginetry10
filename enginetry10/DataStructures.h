#if !defined(DATASTRUCTURES_H)
#include "Helpers.h"
#include <cstdlib>
#include <cstdio>
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
  // void reversePrint();
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
#define DATASTRUCTURES_H
#endif
