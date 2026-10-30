#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _OPENMP
#  include <omp.h>
#endif

// Bessel function of the first kind (order 0):
//   J0(x) = (1/pi) * int_0^pi cos(x*sin(theta)) dtheta
// Each J0(x) is computed with adaptive Simpson quadrature (Lecture 20).
// The x-grid on [0,50] is split among OpenMP threads.

int main(int argc, char* argv[])
{
   void usage(const char* prog_name);
   double AdaptiveInt(const double a,
                      const double b,
                      const double TOL,
                      const double x);

   if (argc != 3) { usage(argv[0]); }
   const int thread_count = strtol(argv[1], NULL, 10);
   const double TOL = strtod(argv[2], NULL);
   if (thread_count<1 || TOL<5.0e-16) { usage(argv[0]); }

   const double pi = 3.141592653589793;
   const double xmin = 0.0;
   const double xmax = 50.0;
   const int N = 2001;                   // number of x points
   const double dx = (xmax-xmin)/((double)(N-1));

   double* x  = (double*)malloc(N*sizeof(double));
   double* J0 = (double*)malloc(N*sizeof(double));

   const double time1 = omp_get_wtime();

   // larger x => more oscillatory integrand => more work, so use dynamic schedule
#  pragma omp parallel for num_threads(thread_count) schedule(dynamic,8)
   for (int i=0; i<N; i++)
   {
      x[i]  = xmin + i*dx;
      J0[i] = AdaptiveInt(0.0, pi, TOL, x[i])/pi;
   }

   const double time2 = omp_get_wtime();

   // write results for plotting
   FILE* file = fopen("bessel.data","w");
   for (int i=0; i<N; i++)
   { fprintf(file,"%24.15e %24.15e\n",x[i],J0[i]); }
   fclose(file);

   // sanity checks: J0(0) = 1, first zero of J0 is near x = 2.404825557695773
   const double z1 = 2.404825557695773;
   const double J0z1 = AdaptiveInt(0.0, pi, TOL, z1)/pi;

   printf("\n");
   printf(" thread_count = %i\n",thread_count);
   printf("          TOL = %24.15e\n",TOL);
   printf("            N = %i points on [%g, %g]\n",N,xmin,xmax);
   printf("        J0(0) = %24.15e\n",J0[0]);
   printf("   J0(%.4f) = %24.15e  (first zero)\n",z1,J0z1);
   printf("       J0(50) = %24.15e\n",J0[N-1]);
   printf("    time used = %12.5e seconds\n",time2-time1);
   printf("\n");

   free(x);
   free(J0);
   return 0;
}

void usage(const char *prog_name)
{
   fprintf(stderr,"usage: %s <num_threads> <TOL>\n",prog_name);
   fprintf(stderr,"   num_threads should be positive\n");
   fprintf(stderr,"   TOL should be positive\n");
   exit(1);
}

double AdaptiveInt(const double a,
                   const double b,
                   const double TOL,
                   const double x)
{
   double Q(const double a, const double b, const double x);

   const double Qab = Q(a,b,x);
   const double c = 0.5*(a+b);
   const double Qac = Q(a,c,x);
   const double Qcb = Q(c,b,x);

   const double one_over_15 = 6.666666666666667e-02;
   const double error_est = one_over_15*fabs(Qac+Qcb-Qab);

   if (error_est<TOL)
   {  return Qac+Qcb;  }
   else
   {
      return AdaptiveInt(a,c,0.5*TOL,x)
           + AdaptiveInt(c,b,0.5*TOL,x);
   }
}

double Q(const double a, const double b, const double x)
{
   double f(const double theta, const double x);
   const double one_sixth = 1.666666666666667e-01;
   const double c = 0.5*(a+b);
   return one_sixth*(b-a)*(f(a,x)+4.0*f(c,x)+f(b,x));
}

double f(const double theta, const double x)
{
   return cos(x*sin(theta));
}
