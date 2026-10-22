#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _OPENMP
# include <omp.h>
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

// Lab 18 - Composite Simpson's rule with OpenMP
// Application: price of a European call option
//   C = e^{-rT} * int_K^{Smax} (S-K) p(S) dS,   Smax = 5*S0
// compared with the exact Black-Scholes formula

// test case parameters
const double S0    = 100.0;
const double K     = 100.0;
const double r     = 0.05;
const double T     = 1.0;
const double sigma = 0.2;

int main(int argc, char* argv[])
{
    void usage(const char* prog_name);
    double CompSimpson(const double a, const double b,
                       const int N, const int thread_count);
    double BlackScholes();

    if (argc != 3) { usage(argv[0]); }
    const int thread_count = strtol(argv[1], NULL, 10);
    const int N = strtol(argv[2], NULL, 10);
    if (thread_count < 1 || N < 2 || N % 2 != 0)
    { usage(argv[0]); }

    const double a = K;
    const double b = 5.0*S0;

    const double time1 = omp_get_wtime();
    double C = exp(-r*T)*CompSimpson(a,b,N,thread_count);
    const double time2 = omp_get_wtime();

    double Cex = BlackScholes();
    double err = fabs(C-Cex);

    printf("\n N = %i, threads = %i\n",N,thread_count);
    printf(" C (Simpson)       = %23.15e\n",C);
    printf(" C (Black-Scholes) = %23.15e\n",Cex);
    printf(" err = %12.5e, time = %12.5e\n\n",err,time2-time1);

    return 0;
}

void usage(const char *prog_name)
{
    fprintf(stderr,"usage: %s <num_threads> <num_intervals>\n",
            prog_name);
    fprintf(stderr," num_threads should be positive\n");
    fprintf(stderr," num_intervals should be positive and even\n");
    exit(1);
}

// Composite Simpson's rule:
// I = h/3 * [ f(x0) + 4f(x1) + 2f(x2) + 4f(x3) + ... + 4f(x_{N-1}) + f(xN) ]
double CompSimpson(const double a, const double b,
                   const int N, const int thread_count)
{
    double func(const double x);
    double h = (b-a)/((double)N);

    double I = func(a) + func(b);
# pragma omp parallel for num_threads(thread_count) \
    reduction(+: I)
    for (int i=1; i<N; i++)
    {
        if (i % 2 == 1)
        { I += 4.0*func(a+i*h); }
        else
        { I += 2.0*func(a+i*h); }
    }

    return h*I/3.0;
}

// integrand: (S-K) * p(S), p(S) is the lognormal pdf of S at maturity
double func(const double S)
{
    const double mu = log(S0) + (r - 0.5*sigma*sigma)*T;
    const double p = 1.0/(S*sigma*sqrt(2.0*M_PI*T))
        * exp( -pow(log(S)-mu,2)/(2.0*sigma*sigma*T) );
    return (S-K)*p;
}

// Phi(x) = 1/2 * (1 + erf(x/sqrt(2)))
double Phi(const double x)
{
    return 0.5*(1.0 + erf(x/sqrt(2.0)));
}

// exact Black-Scholes formula for a European call
double BlackScholes()
{
    double d1 = (log(S0/K) + (r + 0.5*sigma*sigma)*T)/(sigma*sqrt(T));
    double d2 = d1 - sigma*sqrt(T);
    return S0*Phi(d1) - K*exp(-r*T)*Phi(d2);
}
