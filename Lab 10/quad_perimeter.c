#include <stdio.h>
#include <math.h>
#include "quad.h"

/* length of the side between two points */
double distance(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}

void compute_perimeter(quad *q)
{
    printf(" I am in the **compute_perimeter** function.\n");

    double s1 = distance(q->node1.x, q->node1.y, q->node2.x, q->node2.y);
    double s2 = distance(q->node2.x, q->node2.y, q->node3.x, q->node3.y);
    double s3 = distance(q->node3.x, q->node3.y, q->node4.x, q->node4.y);
    double s4 = distance(q->node4.x, q->node4.y, q->node1.x, q->node1.y);

    q->perimeter = s1 + s2 + s3 + s4;
}
