#include "../DataManagementTests.h"

/*
            File Structure

	1 - Math Operations
		1.1 - Generate Random

    2 - Array Operations

    	2.1 - Init
    		2.1.1 - 1D
    		2.1.2 - 3D

    	2.2 - Randomize
    		2.2.1 - 1D
    		2.2.2 - 3D

    	2.3 - Copy
    		2.3.1 - 1D
    		2.3.2 - 3D

    	2.4 - Convert
    		2.4.1 - 1D
    		2.4.2 - 3D

    	2.5 - Comparisons
    		2.5.1 - 1D
    		2.5.2 - 3D

		2.6 - Array Math
			2.6.1 - Pad
			2.6.1 - Flip
*/
static void Print1DMatrix(double* Input, int Dim)
{
	for(int i = 0; i < Dim; ++i)
	{
		printf("%.3f  ", Input[i]);
	}
	printf("\n\n");
}

static void Print3DMatrix(double*** Input, int* Dims)
{
	for(int i = 0; i < Dims[0]; ++i)
	{
		printf("Channel %d\n", i + 1);

		for(int j = 0; j < Dims[1]; ++j)
		{
			for(int z = 0; z < Dims[2]; ++z)
			{
				printf("%.3f ", Input[i][j][z]);
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("\n\n");
}



// 1 --- Math Operations --- //
	
	// 1.1 --- Generate Random --- //

		void GenerateRandTest()
		{
			printf("Starting GenerateRandTest\n\n");

			double Min = -5;
			double Max = 10;

			int Iterations = 10;

			printf("Generating 10 Random Numbers between %.2f and %.2f!\n", Min, Max);
			for(int i = 0; i < Iterations; ++i)
			{
				printf("Randomly Generated Number %d: %.4f\n", i + 1, GenerateRand(Min, Max));
			}

			printf("GenerateRandTest Finished\n\n");
		}

// 2 --- Array Operations --- //

	// 2.1 --- Init  --- //

		// 2.1.1 --- 1D --- //

			void Init1DTest()
			{
				printf("Starting Init1DTest\n\n");

				double DimMin = 1;
				double DimMax = 30;
				int Dim = (int) GenerateRand(DimMin, DimMax);

				double* Input = Init1D(Dim);
				
				printf("Initialized Array (%d):\n", Dim);
				Print1DMatrix(Input, Dim);
				
				Free1D(Input);
				
				printf("Init1D Finished\n\n");
			}

		// 2.1.2 --- 3D --- //

			void Init3DTest()
			{
				printf("Starting Init3DTest\n\n");

				int Dims[3];

				double DimMin = 1;
				double DimMax = 5;

				for(int i = 0; i < 3; ++i)
				{
					Dims[i] = GenerateRand(DimMin, DimMax);
				}

				double*** Input = Init3D(Dims);

				printf("Initialized Array (%d, %d, %d):\n", Dims[0], Dims[1], Dims[2]);
				Print3DMatrix(Input, Dims);

				Free3D(Input);
				
				printf("Init3D Finished\n\n");
			}

	// 2.2 --- Randomize --- //

		// 2.2.1 --- 1D --- //

			void Randomize1DTest()
			{
				printf("Randomize1D Test Starting\n\n");

				double DimMin = 1;
				double DimMax = 30;
				int InDim = (int) GenerateRand(DimMin, DimMax);

				double RandMin = -100;
				double RandMax = 100;

				double* Input = Init1D(InDim);

				printf("Input (%d):\n", InDim);
				Print1DMatrix(Input, InDim);

				RandomizeArray1D(Input, InDim, RandMin, RandMax);

				printf("Randomized Input [%.2f, %.2f]:\n", RandMin, RandMax);
				Print1DMatrix(Input, InDim);

				Free1D(Input);

				printf("Randomize1D Test Finished!\n\n");
			}

		// 2.2.2 --- 3D --- //

			void Randomize3DTest()
			{
				printf("Randomize3D Test Starting\n\n");

				int InDims[3];
				double DimMin = 1;
				double DimMax = 8;

				for(int i = 0; i < 3; ++i)
				{
					InDims[i] = (int) GenerateRand(DimMin, DimMax);
				}

				double RandMin = -100;
				double RandMax = 100;

				double*** Input = Init3D(InDims);

				printf("Input (%d, %d, %d):\n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Input, InDims);

				RandomizeArray3D(Input, InDims, RandMin, RandMax);
				
				printf("Randomized Input [%.2f, %.2f]:\n", RandMin, RandMax);
				Print3DMatrix(Input, InDims);
			
				Free3D(Input);

				printf("Randomize3D Test Finished!\n\n");
			}

	// 2.3 --- Copy --- //

		// 2.3.1 --- 1D --- //

			void Copy1DTest()
			{
				printf("Copy 1D Test Starting\n\n");

				double RandMin = -50;
				double RandMax = 50;

				double DimMin = 1;
				double DimMax = 15;
				int InDim = (int) GenerateRand(DimMin, DimMax);

				double* Input = Init1D(InDim);
				double* Output = Init1D(InDim);

				RandomizeArray1D(Input, InDim, RandMin, RandMax);

				printf("Input (%d) [%.2f, %.2f]:\n", InDim, RandMin, RandMax);
				Print1DMatrix(Input, InDim);
				printf("Output (%d):\n", InDim);
				Print1DMatrix(Output, InDim);
			
				Copy1D(Input, Output, InDim);

				printf("Input (%d):\n", InDim);
				Print1DMatrix(Input, InDim);
				printf("Output (%d):\n", InDim);
				Print1DMatrix(Output, InDim);

				Free1D(Input);
				Free1D(Output);

				printf("Copy 1D Test Finished!\n\n");
			}

		// 2.3.2 --- 3D --- //

			void Copy3DTest()
			{
				printf("Copy3D Test Starting\n\n");

				double RandMin = -25;
				double RandMax = 25;

				int InDims[3];
				double DimMin = 1;
				double DimMax = 8;

				for(int i = 0; i < 3; ++i)
				{
					InDims[i] = (int) GenerateRand(DimMin, DimMax);
				}

				double*** Input = Init3D(InDims);
				double*** Output = Init3D(InDims);

				RandomizeArray3D(Input, InDims, RandMin, RandMax);

				printf("Before Copying:\n");
				printf("Input (%d, %d, %d) [%.2f, %.2f]: \n", InDims[0], InDims[1], InDims[2], RandMin, RandMax);
				Print3DMatrix(Input, InDims);
				printf("Output : \n");
				Print3DMatrix(Output, InDims);
				
				Copy3D(Input, Output, InDims);

				printf("After Copying:\n");
				printf("Input : \n");
				Print3DMatrix(Input, InDims);
				printf("Output : \n");
				Print3DMatrix(Output, InDims);
				
				Free3D(Input);
				Free3D(Output);

				printf("Copy3D Test Finished!\n\n");
			}

	// 2.4 --- Convert --- //

		// 2.4.1 --- 1D --- //

			void ConvertTo1DTest()
			{
				printf("ConvertTo1D Test Starting\n\n");

				double RandMin = -25;
				double RandMax = 25;

				int InDims[3];
				double DimMin = 1;
				double DimMax = 8;

				for(int i = 0; i < 3; ++i)
				{
					InDims[i] = (int) GenerateRand(DimMin, DimMax);
				}

				double*** Input = Init3D(InDims);
				double* Output = Init1D(InDims[0] * InDims[1] * InDims[2]);

				RandomizeArray3D(Input, InDims, RandMin, RandMax);

				printf("Before Convert:\n");
				printf("Input (%d, %d, %d) [%.2f, %.2f]: \n", InDims[0], InDims[1], InDims[2], RandMin, RandMax);
				Print3DMatrix(Input, InDims);
				printf("Output (%d): \n", InDims[0] * InDims[1] * InDims[2]);
				Print1DMatrix(Output, InDims[0] * InDims[1] * InDims[2]);

				ConvertTo1D(Input, Output, InDims);
				
				printf("After Convert:\n");
				printf("Input (%d, %d, %d) [%.2f, %.2f]: \n", InDims[0], InDims[1], InDims[2], RandMin, RandMax);
				Print3DMatrix(Input, InDims);
				printf("Output (%d): \n", InDims[0] * InDims[1] * InDims[2]);
				Print1DMatrix(Output, InDims[0] * InDims[1] * InDims[2]);
				
				Free3D(Input);
				Free1D(Output);

				printf("ConvertTo1D Test Finished!\n\n");
			}

		// 2.4.2 --- 3D --- //

			void ConvertTo3DTest()
			{
				printf("Starting ConvertTo3D Test\n\n");

				double RandMin = -25;
				double RandMax = 25;

				int InDims[3];
				double DimMin = 1;
				double DimMax = 8;

				for(int i = 0; i < 3; ++i)
				{
					InDims[i] = (int) GenerateRand(DimMin, DimMax);
				}

				double* Input = Init1D(InDims[0] * InDims[1] * InDims[2]);
				double*** Output = Init3D(InDims);

				RandomizeArray1D(Input, InDims[0] * InDims[1] * InDims[2], RandMin, RandMax);

				printf("Before Copying:\n");
				printf("Input (%d) [%.2f, %.2f]: \n", InDims[0] * InDims[1] * InDims[2], RandMin, RandMax);
				Print1DMatrix(Input, InDims[0] * InDims[1] * InDims[2]);
				printf("Output (%d, %d, %d): \n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Output, InDims);

				ConvertTo3D(Input, Output, InDims);
				
				printf("After Copying:\n");
				printf("Input (%d) [%.2f, %.2f]: \n", InDims[0] * InDims[1] * InDims[2], RandMin, RandMax);
				Print1DMatrix(Input, InDims[0] * InDims[1] * InDims[2]);
				printf("Output (%d, %d, %d): \n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Output, InDims);

				Free1D(Input);
				Free3D(Output);

				printf("ConvertTo3D Test Complete\n\n");
			}

	// 2.5 --- Compare --- //

		// 2.5.1 --- 1D --- //

			void Compare1DTest()
			{
				printf("Starting Compare1D Test\n\n");

				double RandMin = 5;
				double RandMax = 6;

				double DimMin = 1;
				double DimMax = 30;
				int InDim = GenerateRand(DimMin, DimMax);

				double MarginMin = 0.000001;
				double MarginMax = 0.1;
				double Margin = GenerateRand(MarginMin, MarginMax);

				double* Input1 = Init1D(InDim);
				double* Input2 = Init1D(InDim);

				RandomizeArray1D(Input1, InDim, RandMin, RandMax);
				RandomizeArray1D(Input2, InDim, RandMin, RandMax);
				
				printf("Margin = %.2f\n", Margin);
				printf("Input1 (%d):\n", InDim);
				Print1DMatrix(Input1, InDim);
				printf("Input2 (%d):\n", InDim);
				Print1DMatrix(Input2, InDim);

				Compare1D(Input1, Input2, InDim, Margin);

				Free1D(Input1);
				Free1D(Input2);
				
				printf("Compare Test Complete\n\n");
			}

		// 2.5.2 --- 3D --- //

			void Compare3DTest()
			{
				printf("Starting Compare1D Test\n\n");

				double RandMin = 5;
				double RandMax = 5.5;
				
				double DimMin = 1;
				double DimMax = 3;
				int InDims[3];
				for(int i = 0; i < 3; ++i)
				{
					InDims[i] = GenerateRand(DimMin, DimMax);
				}

				double MarginMin = 0.000001;
				double MarginMax = 0.5;
				double Margin = GenerateRand(MarginMin, MarginMax);

				double*** Input1 = Init3D(InDims);
				double*** Input2 = Init3D(InDims);

				RandomizeArray3D(Input1, InDims, RandMin, RandMax);
				RandomizeArray3D(Input2, InDims, RandMin, RandMax);

				printf("Margin = %.2f\n", Margin);
				printf("Input1 (%d, %d, %d):\n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Input1, InDims);
				printf("Input2 (%d, %d, %d):\n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Input2, InDims);
			
				Compare3D(Input1, Input2, InDims, Margin);

				Free3D(Input1);
				Free3D(Input2);
				
				printf("Compare Test Complete\n\n");
			}

	// 2.6 --- Array Math --- //

		// 2.6.1 --- Pad --- //

			void PadTest()
			{
				printf("Starting Padding Test\n\n");

				double MinPixels = 1;
				double MaxPixels = 5;
				char Pixels = GenerateRand(MinPixels, MaxPixels);

				double RandMin = -100;
				double RandMax = 100;
				
				double DimMin = 1;
				double DimMax = 5;
				int InDims[3];
				int OutDims[3];
				for(int i = 0; i < 2; ++i)
				{
					InDims[i] = GenerateRand(DimMin, DimMax);
				}
				InDims[2] = InDims[1];
				OutDims[0] = InDims[0];
				OutDims[1] = InDims[1] + 2*Pixels;
				OutDims[2] = InDims[2] + 2*Pixels;

				double*** Input = Init3D(InDims);
				double*** Output = Init3D(OutDims);

				RandomizeArray3D(Input, InDims, RandMin, RandMax);
				Pad(Input, Output, InDims, Pixels);

				printf("Pixels = %d\n", (int) Pixels);
				printf("Input (%d, %d, %d) :\n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Input, InDims);
				printf("Output (%d, %d, %d) :\n", OutDims[0], OutDims[1], OutDims[2]);
				Print3DMatrix(Output, OutDims);

				Free3D(Input);
				Free3D(Output);

				printf("Padding Test Complete\n\n");
			}

		// 2.6.2 --- Flip --- //

			void FlipTest()
			{
				printf("Starting Flip Test\n\n");

				double DimMin = 3;
				double DimMax = 5;
				int InDims[3];
				for(int i = 0; i < 2; ++i)
				{
					InDims[i] = GenerateRand(DimMin, DimMax);
				}

				InDims[2] = InDims[1];

				double RandMin = -100;
				double RandMax = 100;

				double*** Input = Init3D(InDims);
				double*** Output = Init3D(InDims);

				RandomizeArray3D(Input, InDims, RandMin, RandMax);

				Flip(Input, Output, InDims);

				printf("Before Flipping\n");
				printf("Input (%d, %d, %d) [%.2f, %.2f]:\n", InDims[0], InDims[1], InDims[2], RandMin, RandMax);
				Print3DMatrix(Input, InDims);
				printf("Output (%d, %d, %d):\n", InDims[0], InDims[1], InDims[2]);
				Print3DMatrix(Output, InDims);

				Free3D(Input);
				Free3D(Output);

				printf("Flip Test Complete\n\n");
			}
