
#include "MTRand.hpp"

unsigned long mt[MTR_N]; /* the array for the state vector  */
int mti=MTR_N+1; /* mti==N+1 means mt[N] is not initialized */
