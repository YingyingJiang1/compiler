/*
#include<stdio.h>
#include<stdlib.h>
#include "ast/ast.h"
int no = 0;

typedef struct Info{
    Node* pNode;
    int no;
    
}Info;

Info* q[100];

int front = 0, rear = 0;
int  isEmpty()
{
    return front == rear ? 1:0;
}

void push(Info* ptr)
{
    q[rear] = ptr;
    ++rear;
}

Info* pop()
{
    ++front;
    return q[front-1];
}

Info* top()
{
    return q[front];
}

Info* newInfo(Node* pNode, int no)
{
    Info* ret = (Info*)malloc(sizeof(Info));
    ret->pNode = pNode;
    ret->no = no;
}



void printAST(Node* t)
{
    
    if(t == NULL)
        return;
    printf("%d:%s:%d    ", no, t->val, -1);
    Info *info = newInfo(t, no);
    push(info);
    while(!isEmpty())
    {
        Info* cur = pop();
        for(int i = 0; i < cur->pNode->num;++i)
        {
            Node* t = cur->pNode->children[i];
            if(t == NULL)
                printf("NULL    ");
            else
            {
                info = newInfo(t, ++no);
                push(info);
                printf("%d:%s:%d    ", no, t->val, cur->no);
            }
            
        }
        printf("\n");
    }
    
}
*/



