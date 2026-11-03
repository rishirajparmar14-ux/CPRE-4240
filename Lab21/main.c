#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

// Lab 21: Pass a Baton
// Rank 0 starts the token at 1000 and passes it around the ring.
// Each rank adds 1 and passes it on, last rank sends it back to rank 0.

int main()
{
   int comm_sz; // number of processors
   int my_rank; // current processor
   int token;

   MPI_Init(NULL,NULL);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   // need at least 2 processes to pass the token
   if (comm_sz < 2)
   {
      if (my_rank==0)
      { printf("Run with at least 2 processes\n"); }
      MPI_Finalize();
      return 1;
   }

   if (my_rank == 0)
   {
      token = 1000;
      MPI_Send(&token,1,MPI_INT,1,0,MPI_COMM_WORLD);
      printf("Rank %d sent token %d to Rank %d\n",my_rank,token,1);

      // wait for the token to come back from the last rank
      MPI_Recv(&token,1,MPI_INT,comm_sz-1,0,
               MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("Rank %d received token %d from Rank %d\n",
             my_rank,token,comm_sz-1);
      printf("Final token = %d\n",token);
   }
   else
   {
      MPI_Recv(&token,1,MPI_INT,my_rank-1,0,
               MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("Rank %d received token %d from Rank %d\n",
             my_rank,token,my_rank-1);

      token = token + 1;

      // last rank sends back to rank 0
      int dest = (my_rank+1) % comm_sz;
      MPI_Send(&token,1,MPI_INT,dest,0,MPI_COMM_WORLD);
      printf("Rank %d sent token %d to Rank %d\n",my_rank,token,dest);
   }

   MPI_Finalize();
   return 0;
}
