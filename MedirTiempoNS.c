#include <stdio.h>
#include <sys/time.h>

int main () {
    double sum = 0;
    double add = 1;
    
    static unsigned long pre_time;
	static unsigned long pre_secs;
	struct timespec ts;
	unsigned long long tiempo;

    // Start measuring time
    clock_gettime(CLOCK_MONOTONIC, &ts);
    pre_time=ts.tv_nsec;
    pre_secs=ts.tv_sec;
    
	//Code to measure
    int iterations = 1000*1000*1000;
    for (int i=0; i<iterations; i++) {
        sum += add;
        add /= 2.0;
    }
    
	// Stop measuring time and calculate the elapsed time
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tiempo=(ts.tv_sec-pre_secs)*1000000000+ts.tv_nsec - pre_time;
    printf("Time measured: %llu\n", tiempo);
    
    return 0;
}