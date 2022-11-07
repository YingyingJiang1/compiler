#include "../ast/ast.h"

void gen3addrCode()
{
    Node* def;
    for(int i = 0; i < root->num; ++i)
    {
        def = root->children[i];
        switch (def->type)
        {
        case FUNC_DEF:
        {
            handleFuncDef(def);
            break;
        }
        default:
            break;
        }
    }
}
