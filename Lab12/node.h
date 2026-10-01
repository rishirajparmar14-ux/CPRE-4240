#ifndef __NODE_H__
#define __NODE_H__

/* one node of the stack: an interval [left,right] that may still
   contain a root of f and has not been processed yet */
typedef struct node node;
struct node
{
    int position;
    double left;
    double right;
    node *next;
};

/* stack operations */
void Push(const double left, const double right, node **top);
void Pop(node **top, double *left, double *right);
void Peek(const node *top, double *left, double *right);
void DisplayStack(const node *top);
void PrintNode(const node *top);
void GetStackSize(const node *top, int *stack_size);
void DeleteStack(node **top);

/* test functions f(x) whose roots we look for */
#define NUM_FUNCTIONS 3
double Function(const int choice, const double x);
const char *FunctionName(const int choice);

/* bisection driven by the stack: finds every root of f on [a,b]
   and stores them in roots[], returns how many were found */
#define MAX_ROOTS 100
int Bisection(const int choice, const double a, const double b,
              const double tol, const double h_min, const int trace,
              double roots[], int *num_steps, int *max_stack);

#endif
