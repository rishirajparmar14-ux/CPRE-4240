#include <stdio.h>
#include <math.h>
#include "quad.h"

#define PI 3.141592653589793

/* inner angle (in degrees) at the corner (xb,yb),
   where (xa,ya) and (xc,yc) are the two neighbouring corners */
double corner_angle(double xa, double ya,
                    double xb, double yb,
                    double xc, double yc)
{
    double ux = xa - xb;
    double uy = ya - yb;
    double vx = xc - xb;
    double vy = yc - yb;

    double dot = ux * vx + uy * vy;
    double lu = sqrt(ux * ux + uy * uy);
    double lv = sqrt(vx * vx + vy * vy);

    double theta = acos(dot / (lu * lv));

    return theta * 180.0 / PI;
}

void compute_angles(quad *q)
{
    printf(" I am in the **compute_angles** function.\n");

    q->angle[0] = corner_angle(q->node4.x, q->node4.y,
                               q->node1.x, q->node1.y,
                               q->node2.x, q->node2.y);

    q->angle[1] = corner_angle(q->node1.x, q->node1.y,
                               q->node2.x, q->node2.y,
                               q->node3.x, q->node3.y);

    q->angle[2] = corner_angle(q->node2.x, q->node2.y,
                               q->node3.x, q->node3.y,
                               q->node4.x, q->node4.y);

    q->angle[3] = corner_angle(q->node3.x, q->node3.y,
                               q->node4.x, q->node4.y,
                               q->node1.x, q->node1.y);
}
