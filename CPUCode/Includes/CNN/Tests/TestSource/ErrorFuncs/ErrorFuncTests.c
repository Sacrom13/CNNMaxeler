#include "../../CNNTests.h"

void ErrorForwardTest()
{
	printf("\nStarting Error Forward Test\n");

	double RandMin = 0;
	double RandMax = 1;

	double DimMin = 1;
	double DimMax = 10;
	int Dim = (int) GenerateRand(DimMin, DimMax);

	char EFunc;
	if(GenerateRand(0,1) < 0.5)
	{
		EFunc = CrossEnt;
	}
	else
	{
		EFunc = MSE;
	}

	double* Prediction = Init1D(Dim);
	double* Truth = Init1D(Dim);

	RandomizeArray1D(Prediction, Dim, RandMin, RandMax);
	RandomizeArray1D(Truth, Dim, RandMin, RandMax);

	double Error = ErrorForward(Prediction, Truth, Dim, EFunc);

	if(Debug)
	{
		if(EFunc == CrossEnt)
		{
			printf("Efunc: CrossEnt\n");
		}
		else
		{
			printf("Efunc: MSE\n");
		}
		
		printf("Prediction (%d) :\n", Dim);
		Print1DMatrix(Prediction, Dim);
		printf("Truth (%d) :\n", Dim);
		Print1DMatrix(Truth, Dim);

		printf("Error : %.2f\n", Error);
	}

	Free1D(Prediction);
	Free1D(Truth);

	printf("\nError Forward Test Complete!\n");
}
	

void ErrorBackwardTest()
{
	printf("\nStarting Error Backward Test\n");
	
	double RandMin = 0;
	double RandMax = 1;

	double DimMin = 1;
	double DimMax = 10;
	int Dim = (int) GenerateRand(DimMin, DimMax);

	char EFunc;
	if(GenerateRand(0,1) < 0.5)
	{
		EFunc = CrossEnt;
	}
	else
	{
		EFunc = MSE;
	}

	double* Prediction = Init1D(Dim);
	double* Truth = Init1D(Dim);

	RandomizeArray1D(Prediction, Dim, RandMin, RandMax);
	RandomizeArray1D(Truth, Dim, RandMin, RandMax);

	double* Output;
	Output = ErrorBackward(Prediction, Truth, Dim, EFunc);

	if(Debug)
	{
		if(EFunc == CrossEnt)
		{
			printf("Efunc: CrossEnt\n");
		}
		else
		{
			printf("Efunc: MSE\n");
		}
		
		printf("Prediction (%d) :\n", Dim);
		Print1DMatrix(Prediction, Dim);
		printf("Truth (%d) :\n", Dim);
		Print1DMatrix(Truth, Dim);

		printf("Error (%d) :\n", Dim);
		Print1DMatrix(Output, Dim);
	}

	Free1D(Prediction);
	Free1D(Truth);
	Free1D(Output);

	printf("\nError Backward Test Complete!\n");
}
		
