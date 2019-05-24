#include "../../CNNTests.h"
/*
                File Structure
	
	1 - Create Network

    2 - Performance
    	2.1 - Classify
    	2.2 - TestAccuracy

	3 - Train
*/

// 1 --- Create Network --- //

	static void CreateNetwork(Network* Net)
	{
		int Dims[3];
		Dims[0] = 1;
		Dims[1] = 28;
		Dims[2] = 28;

		int Freq = 200, Pip = 2, Bs = 64;
		char EFunc = CrossEnt;
		double Lr = 0.01, Mom = 0.02;

		SetLMemFreq(Freq);
		SetDesignFreq(Freq);
		SetPipelining(Pip);

		SetErrorFunc(Net, EFunc);
		SetBatchSize(Net, Bs);
		SetLearningRate(Net, Lr);
		SetMomentum(Net, Mom);

		InitCNN(Net, Dims);

		int NKernels = 32;
		char KernelSize = 3, Stride = 1, Padding = 0, Parallelism = 1;
		char FilterSize = 2, Type = MaxPool;
		char Func = Sigmoid;
		int OutputSize = 4096;
		double DropP = 0.25;

		AddBlock(Net);
		AddConv(NKernels, KernelSize, Stride, Padding, Parallelism);
		AddActi(Func);
		AddConv(NKernels, KernelSize, Stride, Padding, 2*Parallelism);
		AddPool(FilterSize, Type, 2*Stride, Parallelism);
		AddActi(Func);

		AddBlock(Net);
		AddConv(2* NKernels, KernelSize, Stride, Padding, 120*Parallelism);
		AddActi(Func);
		AddConv(2* NKernels, KernelSize, Stride, Padding, 50*Parallelism);
		AddPool(FilterSize, Type, 2*Stride, Parallelism);
		AddActi(Func);

		AddBlock(Net);
		AddFcon(OutputSize, Parallelism);
		AddActi(Func);
		AddDrop(DropP);
		AddFcon(OutputSize, Parallelism);
		AddActi(Func);
		AddDrop(DropP);
		AddFcon(OutputSize, Parallelism);
		AddActi(Func);
		AddDrop(DropP);
		AddFcon(10, Parallelism);
		AddActi(Soft);
	}

// 2 --- Performance --- //

	// 2.1 --- Classify --- //

		void ClassifyTest()
		{
			printf("\nStarting Classify Test\n\n");
			int InDims[3] = {1, 28, 28};
			int RandMin = 0;
			int RandMax = 5;

			double*** Input = Init3D(InDims);
			RandomizeArray3D(Input, InDims, RandMin, RandMax);

			Network* Net = malloc(sizeof(Network));
			CreateNetwork(Net);

			int class = Classify(*Net, Input);

			if(Debug)
			{
				printf("Input Class = %d\n", class + 1);
			}

			Free3D(Input);

			free(Net);

			printf("\nClassify Test Done!\n\n");
		}

	// 2.2 --- CalcTestAccuracy --- //

		void CalcTestAccuracyTest()
		{
			printf("\nStarting CalcTestAccuracy Test\n\n");

			int NSamples = 500;
			int InDims[3] = {1, 28, 28};
			int RandMin = 0;
			int RandMax = 5;

			double**** Inputs = malloc(sizeof(double***) * NSamples);
			double** Labels = malloc(sizeof(double*) * NSamples);

			for(int i = 0; i < NSamples; ++i)
			{
				Inputs[i] = Init3D(InDims);
				RandomizeArray3D(Inputs[i], InDims, RandMin, RandMax);

				Labels[i] = Init1D(10);
				Labels[i][3] = 1;
			}

			Network* Net = malloc(sizeof(Network));
			CreateNetwork(Net);

			double Accuracy = CalcTestAccuracy(*Net, Inputs, Labels, NSamples);

			if(Debug)
			{
				printf("Classification Accuracy = %.2f\n", Accuracy);
			}

			for(int i = 0; i < NSamples; ++i)
			{
				Free3D(Inputs[i]);
				Free1D(Labels[i]);
			}
			free(Inputs);
			free(Labels);

			free(Net);

			printf("\nCalcTestAccuracy Test Done!\n\n");
		}

// 3 --- CNN Train --- //

	void CNNTrainTest()
	{
		printf("\nStarting CNNTrainTest\n\n");

		Network* Net = malloc(sizeof(Network));
		CreateNetwork(Net);

		double**** XTrain;
		double** YTrain;
		double**** XTest;
		double** YTest;

		double Split = 0.3;
		char DataSet = MNIST;

		int DataSetSize = 70000;

		LoadData(&XTrain, &YTrain, &XTest, &YTest, Split, DataSet);

		double GoalError = 0.01;
		double GoalAccuracy = 90;
		int MaxEpochs = 10;
		int TrainDataSize = (1 - Split) * DataSetSize;
		int TestDataSize = Split * DataSetSize;

		if(Debug)
		{
			printf("\nStarting Training\n\n");
		}

		CNNTrainCPU(*Net, XTrain, YTrain, TrainDataSize, MaxEpochs, GoalError, GoalAccuracy);

		if(Debug)
		{
			printf("\nTraining Finished!\n");
		}

		double Accuracy = 0;

		if(Debug)
		{
			printf("Starting Test!\n");
		}

		Accuracy = CalcTestAccuracy(*Net, XTest, YTest, TestDataSize);

		if(Debug)
		{
			printf("Test Accuracy = %.2f%%\n", Accuracy);
		}

		FreeData(XTrain, YTrain, XTest, YTest, DataSet);

		free(Net);

		printf("\nCNNTrainTest Done!\n\n");
	}
