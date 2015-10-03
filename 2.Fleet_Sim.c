#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>

int main(int argc, char** argv)
{
	// Initialize ptstart to store program start time and ptcheck to store program check time
	// ptstart and ptcheck are of type timespec, and include two parts:
	// First part stores seconds
	// Last part stores nanoseconds
	struct timespec ptstart={0,0}, ptcheck={0,0};
	// Get program start time, and store it into where &ptstart pointed
	clock_gettime(CLOCK_MONOTONIC, &ptstart);

	// Define total program running time as 60s
	double const oneMinute = 60.0;
	// Define sampling running time
	// 0.006 = 6ms
	double const interval = 0.006;
	int const root = 0;

	// Initialize total strike counter with 0
	int totalStrikes = 0;
	// Initialize total sample counter with 0
	int totalSamples = 0;

	int size, rank;
	MPI_Init(&argc, &argv);
	// Get total number of threads
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	// Get rank of current thread
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Initialize program timer with 0.0
	double pTimer = 0.0;
	// The outer loop defines the program to run only one minute
	while (pTimer < oneMinute) {
		// Get program check time, and store it into where &ptcheck pointed
		clock_gettime(CLOCK_MONOTONIC, &ptcheck);
		// Calculate how many seconds have past since the program started
		pTimer = ((double)ptcheck.tv_sec + ptcheck.tv_nsec / 1.0e9) - ((double)ptstart.tv_sec + ptstart.tv_nsec / 1.0e9);
		MPI_Bcast(&pTimer, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
		
		// Initialize strike counter with 0
		int strikeCounter = 0;
		// Initialize round counter with 0
		int roundCounter = 0;

		// Initialize ltstart to store loop start time and ltcheck to store loop check time
		// ltstart and ltcheck are of type timespec, and include two parts:
		// First part stores seconds
		// Last part stores nanoseconds
		struct timespec ltstart={0,0}, ltcheck={0,0};
		// Get loop start time, and store it into where &ltstart pointed
		clock_gettime(CLOCK_MONOTONIC, &ltstart);
		// Initialize loop timer with 0.0
		double lTimer = 0.0;
		// The inner loop defines the inner parts to run multiple times in given sampling interval seconds
		while (lTimer < interval) {
			// Get loop check time, and store it into where &ltcheck pointed
			clock_gettime(CLOCK_MONOTONIC, &ltcheck);
			// Calculate how many seconds have past since the loop started
			lTimer = ((double)ltcheck.tv_sec + ltcheck.tv_nsec / 1.0e9) - ((double)ltstart.tv_sec + ltstart.tv_nsec / 1.0e9);
			// printf("%lfs\n", lTimer);
			// fflush(stdout);
			MPI_Bcast(&lTimer, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);

			// Initialize local map's 1100 location counters with all 0
			// Clean counted values inside local 1100 locations from last loop at the same time
			int locMap[1100] = {0};
			// Initialize global map's 1100 location counters with all 0
			// Clean counted values inside global 1100 locations from last loop at the same time
			int gloMap[1100] = {0};

			if (lTimer < interval) {
				// Use a seed that ensure randomness
				// "rank+1" prevent srand(0), ensure rank=0 randomness
				// "(rank+1) *" ensures difference among threads in each round
				// "* clock()" ensures randomness for each thread among different rounds
				// srand ( (rank+1) * clock() );
				srand ( (rank+1) * clock() * (rank+1) * clock() );
				// loc of the vessel, range from 1 to 1100
				int loc = randomize(1100);
				// location map counter
				locMap[loc-1]++;
				// printf("Rank %d, Location %d, Counter %d \n", rank, loc, locMap[loc-1]);
				// fflush(stdout);
			}

			// Sum local maps of all individual threads to global map of root thread
			MPI_Reduce(&locMap, &gloMap, 1100, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

			// Ignore statistical calculation if target interval running time reached
			if (lTimer < interval) {
				// Cumulative counter
				roundCounter++;
			}
			
			// if (rank == root) {
			// Ignore statistical calculation if target interval running time reached
			// root thread perform statistical calculation
			if ((rank == root) && (lTimer < interval)) {
				int i;
				// Iterate all 1100 areas
				for (i = 0; i < 1100; i++) {
					// Only count the location with at least 3 vessels as a strike
					if (gloMap[i] > 2) {
						// printf("Location %d, Counter %d \n", i, gloMap[i]);
						// fflush(stdout);
						// Cumulative counter
						strikeCounter++;
						// printf("Strike %d, Round %d \n", strikeCounter, roundCounter);
						// fflush(stdout);
					}
					
				}
				// printf("SUBTOTAL: \n %d Round \n %d Strikes  \n", roundCounter, strikeCounter);
			}
			// clock_gettime(CLOCK_MONOTONIC, &ltcheck);
			// lTimer = ((double)ltcheck.tv_sec + ltcheck.tv_nsec / 1.0e9) - ((double)ltstart.tv_sec + ltstart.tv_nsec / 1.0e9);
		}
		
		// Ignore statistical calculation if target program running time reached
		if (pTimer < oneMinute) {
			// Cumulative counter
			totalSamples++;
			// root thread perform statistical calculation
			if (rank == root) {
				// Cumulative counter
				totalStrikes = totalStrikes + strikeCounter;
				// printf("TOTAL: \n%d Total Samples \n%d Total Strikes \n%f\nTotal Average \n", totalSamples, totalStrikes, (double)totalStrikes/(double)totalSamples);
				// fflush(stdout);
			}
		}
		// Break out of the loop if target program running time reached
		else {
			break;
		}
	}
	
	// root thread perform statistical calculation
	if (rank == root) {
		// Display statistics result for data analysis
		printf("TOTAL: \n%d Total Samples \n%d Total Strikes \n%f\nTotal Average \n", totalSamples, totalStrikes, (double)totalStrikes/(double)totalSamples);
		// force it to display printf content
		fflush(stdout);
	}
	
	// Cleaning
	MPI_Finalize();
	return 0;
}

// Generate random location coordinates
int randomize(int range) {
	// srand ( rank );
	// Generate and return int range from 1 to range
	return (rand() % range) + 1;
}