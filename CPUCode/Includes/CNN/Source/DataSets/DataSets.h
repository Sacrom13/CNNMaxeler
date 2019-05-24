#ifndef DATASETS_DEFINED
#define DATASETS_DEFINED

	// 1 --- Required Libs --- //

		#include <stdio.h>
		#include <stdlib.h>
		#include <math.h>
		#include "../../Libs/CNNLibs.h"

	// 2 --- DataSets --- //

		#include "MNIST/MNIST.h"

	// 3 --- DataSet Indexes --- //

		#define MNIST 1

	// 4 --- Function Prototypes --- //

		void LoadData(double***** XTrain, double*** YTrain, double***** XTest, double*** YTest, double Split, char DataSet);
		void FreeData(double**** XTrain, double** YTrain, double**** XTest, double** YTest, char DataSet);

#endif
