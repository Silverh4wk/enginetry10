#include "DataStructures.h"
#include "Helpers.h"
#include <stdio.h>
#include <string.h>

int main() {
 /* float A[] = { 67.23,36.2,89,1,37,56,27,82,89,27,515,20,20,20 };
    int ArraySize = ArrayCount(A);
    MergeSort(A,0,ArraySize-1);
 */   
   
    std::list<double> List = { 10,2,5,6,4,8,1,2,3,5,48,4 };
    QuickSort(List);
    for (auto x : List)
        std::cout << x << " ";
    

   /* Node* NewNode= new Node(10);
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
    list.forwardPrint();*/




    return 0;
}
