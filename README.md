# MPI.Fleet
Distributed Event Modelling with MPI
User defined number of naval fleet patrols 1100 distinct locations randomly at the end of each sampling interval.
1. At least 3 vessels need to be in the same location, at a given point in time, for 1 strike to be counted.
2. The fleet might generate more than 1 strike at the end of each sampling interval
3. The objective is to achieve the highest possible strike rate.
    Increase the number of vessels can increase the probability of at least 3 vessels in the same location
    But higher inter-process communication overheads can also slow the program.
