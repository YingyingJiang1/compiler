#ifndef TREE_NODE_H
#define TREE_NODE_H

#define VAL_SIZE 40
#define CHILDREN_SIZE 3
/*token type of node */
typedef enum NodeType
{
    ARITHMATIC_OP = 1,
    RELOP_OP,
    LOGICAL_OP,
    ASSIGN_OP,
    MEMBER_ACCESS_OP,
    BASE_TYPE,  //int, float
    STRUCT_TYPE,    //struct
    IDENTIFIER,     // name of var, struct, func
    INTEGER,        // constant int
    FLOAT_POINT = 10,   // constant float
    INT_TYPE,        //int type
    FLOAT_TYPE,    // float type
    TOKEN_OTHER,    // comma...
    BRANCH,         // if
    WHILE_LOOP,     // while
    ARRAY_REFERENCE,// use type of array, such like a[3][4]
    LOCAL_DEF,     
    LOCAL_DEF_LIST,
    EXT_DEF_LIST,
    GLOBAL_VAR_DEF = 20,
    FUNC_CALL,    
    FUNC_DEF,
    FUNC_DEC,
    CALL_RETURN,
    STMT_LIST,
    COMP_ST,

}NodeType;

typedef struct Node
{
    int lineNo;/* line of the syntax unit*/
    NodeType type;/* type of the token*/
    char val[VAL_SIZE];
    int num; // the number of children
    struct Node** children; 
}Node;

extern Node* root;


Node* genNode(int lineNo, NodeType type, char* val);
Node* createAST(Node* root, int argc,...);
void mergeNodes(Node* t, NodeType type);
void deleteTree(Node* t);

#endif