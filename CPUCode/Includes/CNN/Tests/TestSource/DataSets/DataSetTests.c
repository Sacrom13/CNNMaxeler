#include "../../CNNTests.h"

// 1 --- Data --- //
	
	void DataTest()
	{
		printf("Starting DataTest!\n\n");
		char DataSet = MNIST;
		double Split = 0.3;

		double**** XTrain;
		double**** XTest;
		double** YTrain;
		double** YTest;

		LoadData(&XTrain, &YTrain, &XTest, &YTest, Split, DataSet);

		int iterations = 10;

		for(int i = 0; i < iterations; ++i)
		{
			int Sample = round(GenerateRand(0, 70000 * Split));
			printf("Iteration %d ->", i + 1);
			printf("Test Label[%d]:\n", Sample + 1);
			Print1DMatrix(YTest[Sample], 10);
		}

		FreeData(XTrain, YTrain, XTest, YTest, DataSet);

		printf("DataTest Complete!\n\n");
	}
