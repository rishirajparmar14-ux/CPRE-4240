#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

// Pass a Baton (Lecture 21: Introduction to MPI)
//   Rank 0 starts a token = 1000 and sends it to Rank 1.
//   Each rank r (1 <= r <= size-1) receives the token from rank r-1,
//   increments it by 1, and sends it to rank (r+1) % size.
//   The last rank (size-1) therefore sends the token back to Rank 0,
//   which prints the final value: 1000 + (size-1).

int main()
{
   int comm_sz;  // number of processors
   int my_rank;  // current processor

   MPI_Init(NULL,NULL);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   if (comm_sz < 2)
   {
      if (my_rank==0)
      { fprintf(stderr,"Error: run with at least 2 processes (mpiexec -n 4 ...)\n"); }
      MPI_Finalize();
      return 1;
   }

   const int next = (my_rank+1) % comm_sz;           // who I send to
   const int prev = (my_rank-1+comm_sz) % comm_sz;   // who I receive from
   int token;

   if (my_rank==0)
   {
      // Start: Rank 0 initializes the token and makes the first pass
      token = 1000;
      MPI_Send(&token,1,MPI_INT,next,0,MPI_COMM_WORLD);
      printf("Rank %d sent token %d to Rank %d\n",my_rank,token,next);
      fflush(stdout);

      // Completion: wait for the token to come back around the ring
      MPI_Recv(&token,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("Rank %d received token %d from Rank %d\n",my_rank,token,prev);
      printf("Rank 0: final token = %d (expected %d) -> %s\n",
             token, 1000+(comm_sz-1),
             token==1000+(comm_sz-1) ? "full trip verified" : "MISMATCH");
      fflush(stdout);
   }
   else
   {
      // The Pass: receive from previous rank, increment, forward to next rank
      MPI_Recv(&token,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("Rank %d received token %d from Rank %d\n",my_rank,token,prev);
      fflush(stdout);

      token += 1;

      MPI_Send(&token,1,MPI_INT,next,0,MPI_COMM_WORLD);
      printf("Rank %d sent token %d to Rank %d\n",my_rank,token,next);
      fflush(stdout);
   }

   MPI_Finalize();
   return 0;
}
