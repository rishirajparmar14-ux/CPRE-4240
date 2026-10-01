#include <stdio.h>
#include "node.h"

void DisplayStack(const node* top)
{
    if (top == NULL)
    { printf(" Stack is empty.\n"); return; }

    printf(" Pos   Left   Right\n");
    PrintNode(top);
}

void PrintNode(const node* top)
{
    printf(" %i   %f   %f\n", top->position, top->left, top->right);
    if (top->next == NULL)
    { return; }
    PrintNode(top->next);
}
