#include <stdio.h>
#include "node.h"

void DisplayStack(const node *top)
{
    if (top == NULL)
    {
        printf(" Stack is empty.\n");
        return;
    }

    printf(" -------------------------------------------------------------------------\n");
    printf(" |Pos:|     Left:     |    Right:     |     Address:     |      Next:       |\n");
    printf(" -------------------------------------------------------------------------\n");
    PrintNode(top);
    printf(" -------------------------------------------------------------------------\n");
}

void PrintNode(const node *top)
{
    printf(" |%3i | %13.8f | %13.8f | %16p | %16p |\n",
           top->position, top->left, top->right, (void *)top, (void *)top->next);
    if (top->next == NULL)
    {
        return;
    }
    PrintNode(top->next);
}
