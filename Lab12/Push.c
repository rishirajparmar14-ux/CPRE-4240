#include <stdio.h>
#include <stdlib.h>
#include "node.h"

void Push(const double left, const double right, node** top)
{
    node* temp = (node*)malloc(sizeof(struct node));
    temp->left = left;
    temp->right = right;
    temp->position = 1;
    temp->next = *top;
    *top = temp;

    // update positions of the nodes below
    node* ptr = (*top)->next;
    while (ptr != NULL)
    {
        ptr->position = ptr->position + 1;
        ptr = ptr->next;
    }
}
