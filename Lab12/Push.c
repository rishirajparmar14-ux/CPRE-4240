#include <stdio.h>
#include <stdlib.h>
#include "node.h"

void Push(const double left, const double right, node **top)
{
    node *temp = (node *)malloc(sizeof(struct node));
    if (temp == NULL)
    {
        printf("\n Error: out of memory in Push.\n");
        exit(1);
    }
    temp->left = left;
    temp->right = right;
    temp->position = 1;
    temp->next = *top;
    *top = temp;

    /* every node under the new top moves down one position */
    node *ptr = (*top)->next;
    while (ptr != NULL)
    {
        ptr->position = ptr->position + 1;
        ptr = ptr->next;
    }
}
