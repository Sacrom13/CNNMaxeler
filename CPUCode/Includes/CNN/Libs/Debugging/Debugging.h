#ifndef DEBUGGING_DEFINED
#define DEBUGGING_DEFINED

	// 1 --- Required Libs --- //

		#include "Libs/DebuggingLibs.h"

	// 2 --- Global Variables --- //

		extern char Debug;

	// 3 --- Function Prototypes --- //

		void ToggleDebugMode();
		void Print1DMatrix(double* Input, int Dim);
		void Print3DMatrix(double*** Input, int* Dims);

#endif
