#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Node
{
    int index;
    int val;
    struct Node * next;
} list;
typedef struct Node2
{
    int index;
    int val;
    int ref;
    int dir;
    struct Node2 * next;
} second;
/*
typedef struct Node3
{
    int index;
    int val;
    int ref;
    struct Node3 * next;
} segment;
*/
void print_list(list * head);
void print_list2(second * head);
//void print_segment(segment* head);
void push(list* head, int index,int val);
void push2(second* head, int index,int val,int ref,int dir);
//void pusha(segment* head,int index,int val,int ref);
int search(list* head, int x);
//int searchact(segment* head,int val);
int search2(second* head, int x);
int searchpoint(second* head, int ref,int dir,int start);
void ref2zero(second* head,int startpoint,int z);
int active[100];
int inactive[100];
int ref[200];
int inactiven,activen;
int searcharr (int  arr[],int n,int k);
void printarr(int arr[],int n);
void movetohead(int arr[],int k);
void deleten(int arr[],int n,int k);