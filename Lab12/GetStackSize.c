#include <stdlib.h>
#include "node.h"

/* the bottom node's position is the number of nodes in the stack */
void GetStackSize(const node *top, int *stack_size)
{
    if (top == NULL)
    {
        *stack_size = 0;
        return;
    }

    if (top->next == NULL)
    {
        *stack_size = top->position;
        return;
    }
    GetStackSize(top->next, stack_size);
}
