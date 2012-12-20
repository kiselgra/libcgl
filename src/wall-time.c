#include "wall-time.h"

#include <sys/time.h>
#include <unistd.h>

double wall_time_in_ms() {
	struct timeval tv;
	gettimeofday(&tv, 0);
	double ret = (double)tv.tv_sec*1000.0 + (double)tv.tv_usec/1000.0;
	return ret;
}
