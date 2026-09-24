#include <stdio.h>
#include <math.h>
#include "quad.h"

/* area of the triangle with corners (x1,y1), (x2,y2) and (x3,y3) */
double triangle_area(double x1, double y1,
                     double x2, double y2,
                     double x3, double y3)
{
    double ux = x2 - x1;
    double uy = y2 - y1;
    double vx = x3 - x1;
    double vy = y3 - y1;

    double cross = ux * vy - uy * vx;

    return 0.5 * fabs(cross);
}

void compute_area(quad *q)
{
    printf(" I am in the **compute_area** function.\n");

    /* split the quadrilateral into two triangles along the diagonal
       from node1 to node3 and add up the two areas */
    double a1 = triangle_area(q->node1.x, q->node1.y,
                              q->node2.x, q->node2.y,
                              q->node3.x, q->node3.y);

    double a2 = triangle_area(q->node1.x, q->node1.y,
                              q->node3.x, q->node3.y,
                              q->node4.x, q->node4.y);

    q->area = a1 + a2;
}
