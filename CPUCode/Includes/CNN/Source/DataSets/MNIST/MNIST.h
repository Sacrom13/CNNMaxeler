#ifndef MNIST_DEFINED
#define MNIST_DEFINED
	
	// 1 --- Function Prototypes --- //

		void LoadDataMNIST(double***** XTrain, double*** YTrain, double***** XTest, double*** YTest, double Split);
		void FreeDataMNIST(double**** XTrain, double** YTrain, double**** XTest, double** YTest);
		
#endif
