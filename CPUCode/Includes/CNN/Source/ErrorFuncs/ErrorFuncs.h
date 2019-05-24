#ifndef EFUNCS_DEFINED
#define EFUNCS_DEFINED

	// 1 --- Required Libs --- //

		#include <math.h>
		#include "../../Libs/CNNLibs.h"


	// 2 --- Function Prototypes --- //

		double ErrorForward(double* Prediction, double* Truth, int Dim, char EFunc);
		double* ErrorBackward(double* Prediction, double* Truth, int Dim, char EFunc);

#endif
