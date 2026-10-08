#include <stdlib.h>
#include "poly.h"

/* free every node in the list */
void DeletePoly(term **head)
{
    term *temp;

    while (*head != NULL)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}
