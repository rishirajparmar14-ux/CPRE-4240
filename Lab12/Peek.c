#include "node.h"

void Peek(const node* top, double* left, double* right)
{
    *left = top->left;
    *right = top->right;
}
