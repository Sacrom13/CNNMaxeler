#include "../TimingTests.h"

/*
            File Structure

	1 - Timing

*/

// 1 --- Timing --- //

	void TimingTest()
	{
		printf("Starting Timing Test!\n\n");

		int Time = 2;
		
		printf("Sleeping for %d seconds\n", Time);

		StartTiming();

		sleep(Time);

		double Total = StopTiming();

		printf("Time Elapsed : %.2f ms\n", Total/1000);

		printf("Timing Test Complete!\n\n");
	}