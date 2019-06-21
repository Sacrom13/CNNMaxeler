#ifndef EFUNCS_DEFINED
#define EFUNCS_DEFINED

	// 1 --- Required Libs --- //

		#include <math.h>
		#include "../../Libs/CNNLibs.h"

	// 2 --- Error Func ID's --- //

		#define CrossEnt 1
		#define MSE 2

	// 3 --- Function Prototypes --- //

		double ErrorForward(double* Prediction, double* Truth, int Dim, char EFunc);
		double* ErrorBackward(double* Prediction, double* Truth, int Dim, char EFunc);

#endif
