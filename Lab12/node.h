#ifndef __NODE_H__
#define __NODE_H__

// each node stores an interval [left,right] that might have a root
typedef struct node node;
struct node
{
    int position;
    double left;
    double right;
    node* next;
};

// stack functions
void Push(const double left, const double right, node** top);
void Pop(node** top, double* left, double* right);
void Peek(const node* top, double* left, double* right);
void DisplayStack(const node* top);
void PrintNode(const node* top);
void GetStackSize(const node* top, int* stack_size);
void DeleteStack(node** top);

// function and bisection
double f(const int choice, const double x);
int Bisection(const int choice, const double a, const double b,
              const double tol, const double h_min, const int show,
              double roots[], int* steps);

#endif
