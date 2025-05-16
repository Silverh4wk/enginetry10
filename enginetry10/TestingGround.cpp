#include "DataStructures.h"
#include "Helpers.h"
#include <stdio.h>
#include <string.h>

int main() {
  //int A[] = { 67,36,89,1,37,56,27,82,89,27 };
  //  int ArraySize = ArrayCount(A);
  //  DescHeapSort(A,ArraySize);

  //  printf("Array after sorting: ");
  //  char Buffer[256];
  //  Buffer[0] = '\0'; 

  //  for (int i = 0; i < ArraySize; i++) {
  //      char Temp[32];
  //      sprintf_s(Temp, sizeof(Temp), "%d, ", A[i]);
  //      strcat_s(Buffer, sizeof(Buffer), Temp);
  //  }

  //  printf("%s", Buffer); 
    
    Node* NewNode= new Node(10);
    Node* NextNode = new Node(20);

    NodeForward(NewNode, NextNode);
    LinkedList list;
    Node* temp = NewNode;
    while (temp != NULL) {
        printf("%d ", temp->Data);
        temp = temp->Next;
    }
    printf("\n");
   
    list.push_front(NewNode->Data);
    list.push_front(NextNode->Data);
    list.forwardPrint();




    return 0;
}
