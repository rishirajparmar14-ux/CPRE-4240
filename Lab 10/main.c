#include <stdio.h>
#include "quad.h"

int main()
{
    quad q;

    printf("\n Enter the four corners of the quadrilateral in order\n");
    printf(" (going around the shape).\n\n");

    printf(" node #1 (x y): ");
    scanf("%lf %lf", &q.node1.x, &q.node1.y);

    printf(" node #2 (x y): ");
    scanf("%lf %lf", &q.node2.x, &q.node2.y);

    printf(" node #3 (x y): ");
    scanf("%lf %lf", &q.node3.x, &q.node3.y);

    printf(" node #4 (x y): ");
    scanf("%lf %lf", &q.node4.x, &q.node4.y);

    compute_perimeter(&q);
    compute_area(&q);
    compute_angles(&q);

    printf("\n node #1: ( %8.4f, %8.4f )\n", q.node1.x, q.node1.y);
    printf(" node #2: ( %8.4f, %8.4f )\n", q.node2.x, q.node2.y);
    printf(" node #3: ( %8.4f, %8.4f )\n", q.node3.x, q.node3.y);
    printf(" node #4: ( %8.4f, %8.4f )\n", q.node4.x, q.node4.y);

    printf("\n perimeter = %10.4f\n", q.perimeter);
    printf(" area      = %10.4f\n", q.area);

    double sum = 0.0;

    printf("\n inner angles (degrees):\n");
    for (int i = 0; i < 4; i++)
    {
        printf("   angle at node #%i = %10.4f\n", i + 1, q.angle[i]);
        sum = sum + q.angle[i];
    }
    printf("   sum of the angles = %10.4f\n\n", sum);

    return 0;
}
