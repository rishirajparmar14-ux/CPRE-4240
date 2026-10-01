#include <stdlib.h>
#include "node.h"

void Pop(node** top, double* left, double* right)
{
    node* temp = *top;

    if (temp == NULL)
    { return; }
    else
    { temp = temp->next; }

    *left = (*top)->left;
    *right = (*top)->right;
    free(*top);
    *top = temp;

    node* ptr = *top;
    while (ptr != NULL)
    {
        ptr->position = ptr->position - 1;
        ptr = ptr->next;
    }
}
