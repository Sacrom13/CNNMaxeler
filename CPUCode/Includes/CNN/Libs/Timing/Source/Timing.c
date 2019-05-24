#include "../Timing.h"

/*

			File Structure

	1 - Global Variables

	2 - Execution Time
		2.1 - Start Timing
		2.2 - Stop Timing

*/

// 1 --- Global Variables --- //

	static struct timeval st, et;

// 2 --- Execution Time --- //

	// 2.1 --- Start Timing --- //

		/*
			Starts Timing

			Return Value - Nothing
		*/

		void StartTiming()
		{
			gettimeofday(&st,NULL);
		}

	// 2.2 --- Stop Timing --- //

		/*
			Stops Timing

			Return Value - Elapsed time between Call of StartTiming and StopTiming
		*/
		
		double StopTiming()
		{
			gettimeofday(&et,NULL);
			return ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
		}