#include<string.h>
#include<assert.h>
#include <stdlib.h>
#include<stdio.h>
#include<stdarg.h>
#include "ast.h"

Node* genNode(int lineNo, NodeType type, char* val)
{
    Node* ptr = (Node*)malloc(sizeof(Node));
    memset(ptr, 0, sizeof(Node));
    ptr->lineNo = lineNo;
    ptr->type = type;
    if(strlen(val) + 1 > VAL_SIZE)
        exit(-1);
    strncpy(ptr->val, val, strlen(val)+1);
    ptr->num = 0;
    ptr->children = NULL;
    return ptr;
}



Node* createAST(Node* root, int argc,...)
{
    int num = root->num;
    Node* tmp;
    Node** newArr = (Node**)malloc(sizeof(Node*) * (num+argc));
    if(num > 0)
        memcpy(newArr, root->children, sizeof(Node*)*num);
    if(root->children)
        free(root->children);

    va_list valist;
    va_start(valist, argc);
    // check whether the first arg is NULL
    tmp = va_arg(valist, Node*);
    if(tmp)
    {
        newArr[num] = tmp;
    }
        
    else
        --num;  // first arg is NULL
    
    for(int i = 1; i < argc; ++i)
    {
        newArr[i+num] = va_arg(valist, Node*);
    }
    root->children = newArr;
    root->num = num + argc;    
    // check whether the last arg is NULL
    if(root->children[root->num-1] == NULL)
        --root->num;    
    return root;
}

void freeNode(Node* ptr)
{
    if(ptr->children)
        free(ptr->children);
    free(ptr);
}

/* including func_call(delete all comma node) , array_reference(delete surplus arrayRef node)
LOCAL_DEF(delete surplus LOCAL_DEF node)...
Store the useful nodes of the subtrees to be merged in an array, and copy the values ​​in the array to the root of tree t.
*/
void mergeNodes(Node* t, NodeType type)
{   
    if(t == NULL)
        return;
    int count = 0;
    Node* arr[10000];
    // if the root of t has only one child, merge the tree whose root is the child of the root of t.
    //if the root of t has two children, the first child of root is useful so merge the second.
    
    Node* cur, *pre;
    if(type == ARRAY_REFERENCE)
    {        
        cur = t->children[0];
        if(cur->type != ARRAY_REFERENCE)
        {
            return;
        }
            
        Node** newArr = (Node**)malloc(sizeof(Node*)*(cur->num + 1));
        memcpy(newArr, cur->children, sizeof(Node*)*cur->num);
        newArr[cur->num] = t->children[1];
        t->num = cur->num + 1;
        freeNode(cur);
        free(t->children);
        t->children = newArr;
        return;
    }
    
    
    assert(t->num <= 2);
    cur = t->num == 1?t->children[0]:t->children[1];
    if(!cur || cur->type != type)
    {      
        return;
    }
        
    while(cur && cur->type == type)
    {
        assert(count < 10000);
        arr[count++] = cur->children[0];
        pre = cur;
        cur = cur->children[1];
        freeNode(pre);
    }
    // must check whether cur is NULL
    if(cur)
        arr[count++] = cur;
    // subtree or right subtree are transform into a array of Node*    
    --t->num;
    Node** newArr = (Node**)malloc(sizeof(Node*)*(count + t->num));
    if(t->num == 1)
        newArr[0] = t->children[0];
    memcpy(newArr+t->num, arr, sizeof(Node*)*count);

    t->num += count;
    free(t->children);
    t->children = newArr;
}


