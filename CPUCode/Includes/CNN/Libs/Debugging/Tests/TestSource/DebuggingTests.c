#include "../DebuggingTests.h"

/*
            File Structure

	1 - Toggle Debug
	
	2 - Print
		2.1 - 1D
		2.2 - 3D

*/

// 1 --- Toggle Debug --- //

	void ToggleDebugTest()
	{
		printf("Starting Toggle Debug Test!\n\n");

		int iterations = 10;

		for(int i = 0; i < iterations; ++i)
		{
			printf("Iteration %d : ", i + 1);
			printf("Before Toggle, ");
			printf("Debug : %d, ", Debug);
			ToggleDebugMode();
			printf("After Toggle, ");
			printf("Debug : %d\n", Debug);
		}
		
		printf("Debugging Test Done!\n\n");
	}

// 2 --- Print --- //

	// 2.1 --- 1D --- //

		void Print1DMatrixTest()
		{
			printf("Starting Print1DTest!\n\n");

			int InDim = 10;

			double* Input = calloc(InDim, sizeof(double));

			Print1DMatrix(Input, InDim);

			free(Input);

			printf("Print1DTest Done!\n\n");
		}


	// 2.2 --- 3D --- //

		void Print3DMatrixTest()
		{
			printf("Print3D Test Starting\n\n");

			int Dims[3];
			Dims[0] = 3;
			Dims[1] = 4;
			Dims[2] = 4;

			double*** Input = (double***) calloc( (sizeof(double**) * Dims[0]) + (sizeof(double*) * Dims[0] * Dims[1]) + (sizeof(double) * Dims[0] * Dims[1] * Dims[2]) , 1);

			for(int i = 0; i < Dims[0]; ++i)
			{
				Input[i] = (double**)(Input + Dims[0]) + i * Dims[1];
				for(int j = 0; j < Dims[1]; ++j)
				{
					Input[i][j] = (double*)(Input + Dims[0] + Dims[0]*Dims[1]) + i*Dims[1]*Dims[2] + j*Dims[2];
				}
			}

			Print3DMatrix(Input, Dims);
		
			free(Input);

			printf("Print3D Test Finished!\n\n");
		}
