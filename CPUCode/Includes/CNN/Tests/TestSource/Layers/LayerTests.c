#include "../../CNNTests.h"

// 1 --- Layer Tests --- //

	// 1.1 --- Conv --- //

		// 1.1.1 --- Forward --- //

			void ConvForwTest()
			{
				printf("Starting Conv Test\n\n");

				double RandMin = -5;
				double RandMax = 5;

				char Act = ReLu;
				int NKernels = 32;
				char KernelSize = 3, Stride = 1, Padding = 0;

				double Params[5] = {Act, NKernels, KernelSize, Stride, Padding};

				int NChannels = 3;
				int InDim = 28;

				double Margin = 0.5e-3;

				int InDims[3];
				InDims[0] = NChannels;
				InDims[1] = InDim;
				InDims[2] = InDims[1];

				int OutDims[3];
				OutDims[0] = NKernels;
				OutDims[1] = 1 + ((InDims[1] - KernelSize + 2 * Padding)/Stride);
				OutDims[2] = OutDims[1];

				int FiltDims[3];
				FiltDims[0] = NChannels;
				FiltDims[1] = (int) KernelSize;
				FiltDims[2] = FiltDims[1];
				

				FILE* InputDataFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Conv/TestData.txt", "r");
				FILE* WeightFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Conv/TestWeights.txt", "r");
				FILE* OutputDataFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Conv/TestOutput.txt", "r");

				double*** Input = Init3D(InDims);
				for(int channel = 0; channel < InDims[0]; ++channel)
				{
					for(int y = 0; y < InDims[1]; ++y)
					{
						for(int x = 0; x < InDims[2]; ++x)
						{
							fscanf(InputDataFile, "%lf", &Input[channel][y][x]);
						}
					}
				}

				double**** Filters = malloc(sizeof(double***) * NKernels);
				for(int i = 0; i < NKernels; ++i)
				{
					Filters[i] = Init3D(FiltDims);
					for(int j = 0; j < FiltDims[0]; ++j)
					{
						for(int k = 0; k < FiltDims[1]; ++k)
						{
							for(int l = 0; l < FiltDims[2]; ++l)
							{
								fscanf(WeightFile, "%lf", &Filters[i][j][k][l]);
							}
						}
					}
				}

				double*** TestOutput = Init3D(OutDims);
				for(int channel = 0; channel < OutDims[0]; ++channel)
				{
					for(int y = 0; y < OutDims[1]; ++y)
					{
						for(int x = 0; x < OutDims[2]; ++x)
						{
							fscanf(OutputDataFile, "%lf", &TestOutput[channel][y][x]);
						}
					}
				}

				double*** Output = Init3D(OutDims);

				ConvForwCpu(Input, InDims, Output, Filters, Params);
					
				if(Debug)
				{
					printf("RandMin: %.2f, RandMax:%.2f\n", RandMin, RandMax);
					printf("Input:\n");
					Print3DMatrix(Input, InDims);

					printf("Filters:\n");
					for(int i = 0; i < NKernels; ++i)
					{
						Print3DMatrix(Filters[i], FiltDims);
					}

					printf("Output:\n");
					Print3DMatrix(Output, OutDims);
				}


				Compare3D(Output, TestOutput, OutDims, Margin);

				Free3D(Input);

				for(int i = 0; i < NKernels; ++i)
				{
					Free3D(Filters[i]);
				}
				free(Filters);

				Free3D(Output);
				Free3D(TestOutput);

				printf("Conv Test Complete\n\n");
			}

		// 1.1.2 --- Backward --- //

			void ConvBackTest()
			{
				printf("Starting Conv Backward Test\n\n");

				char Act = ReLu;
				int NKernels = 5;
				char KernelSize = 3, Stride = 2, Padding = 1;

				double LearningRate = 0.01;

				int NChannels = 3, InDim = 5;

				int InDims[3];
				InDims[0] = NChannels;
				InDims[1] = InDim;
				InDims[2] = InDims[1];

				int OutDims[3];
				OutDims[0] = NKernels;
				OutDims[1] = 1 + ((InDims[1] - KernelSize + 2*Padding) / Stride);
				OutDims[2] = OutDims[1];

				double*** PrevInput = Init3D(InDims);							// Input during Forward Prop
				RandomizeArray3D(PrevInput, InDims, 0, 3);

				double*** PrevOutput = Init3D(OutDims);							// Output during Forward Prop
				RandomizeArray3D(PrevOutput, OutDims, 0, 3);

				double*** Error = Init3D(OutDims);								// Error from previous layer
				RandomizeArray3D(Error, OutDims, 0, 3);

				double*** Output = Init3D(InDims);								// Output of BackProp

				int FiltDims[3];
				FiltDims[0] = NChannels;
				FiltDims[1] = (int) KernelSize;
				FiltDims[2] = FiltDims[1];

				double**** Filters = malloc(sizeof(double***) * NKernels);		// Kernel Weights
				for(int i = 0; i < NKernels; ++i)
				{
					Filters[i] = Init3D(FiltDims);
					RandomizeArray3D(Filters[i], FiltDims, 0, 3);
				}

				double Params[5] = {Act, NKernels, KernelSize, Stride, Padding};

				ConvBackCpu(PrevInput, InDims, PrevOutput, OutDims, Error, Output, Filters, Params, LearningRate);

				if(Debug)
				{
					printf("Params:\n");
					if(Act == ReLu)
					{
						printf("Act: Relu, ");
					}
					else if(Act == Sigmoid)
					{
						printf("Act: Sigmoid, ");
					}
					else
					{
						printf("Act: Tanh, ");
					}
					printf("NKernels: %d, KernelSize: %d, Stride: %d, Padding: %d\n", NKernels, KernelSize, Stride, Padding);

					printf("Prev Input:\n");
					Print3DMatrix(PrevInput, InDims);

					printf("PrevOutput:\n");
					Print3DMatrix(PrevOutput, OutDims);

					printf("Error:\n");
					Print3DMatrix(Error, OutDims);

					printf("Output:\n");
					Print3DMatrix(Output, InDims);

					printf("Filters:\n\n");
					for(int i = 0; i < NKernels; ++i)
					{
						printf("Filter %d\n", i+1);
						Print3DMatrix(Filters[i], FiltDims);
					}

				}

				Free3D(PrevInput);
				Free3D(PrevOutput);
				Free3D(Error);
				Free3D(Output);

				for(int i = 0; i < NKernels; ++i)
				{
					Free3D(Filters[i]);
				}
				free(Filters);

				printf("Conv Back Test Done!\n");
			}

	// 1.2 --- Fcon --- //

		// 1.2.1 --- Forward --- //

			void FconForwTest()
			{
				printf("Starting Fcon Test\n\n");

				char Act = ReLu, Outputs = 4;
				double DropP = 0;

				int InDim = 128, OutDim = 256;
				double Params[3] = {Act, DropP, Outputs};

				double Margin = 0.5e-3;

				FILE* InputDataFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Fcon/TestData.txt", "r");
				FILE* WeightFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Fcon/TestWeights.txt", "r");
				FILE* OutputDataFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Fcon/TestOutput.txt", "r");


				double* Input = Init1D(InDim);
				for(int i = 0; i < InDim; ++i)
				{
					fscanf(InputDataFile, "%lf", &Input[i]);
				}

				double** Weights = malloc(sizeof(double*) * InDim);
				for(int i = 0; i < InDim; ++i)
				{
					Weights[i] = Init1D(OutDim);
					for(int j = 0; j < OutDim; ++j)
					{
						fscanf(WeightFile, "%lf", &Weights[i][j]);
					}
				}

				double* TestOutput = Init1D(OutDim);
				for(int i = 0; i < OutDim; ++i)
				{
					fscanf(OutputDataFile, "%lf", &TestOutput[i]);
				}

				double* Output = Init1D(OutDim);

				FconForwCpu(Input, InDim, Output, OutDim, Weights, Params, 1);

				if(Debug)
				{
					printf("Params:\n");
					if(Act == ReLu)
					{
						printf("Act: Relu, ");
					}
					else if(Act == Sigmoid)
					{
						printf("Act: Sigmoid, ");
					}
					else
					{
						printf("Act: Tanh, ");
					}
					printf("DropP: %.2f\n", DropP);

					printf("Input:\n");
					Print1DMatrix(Input, InDim);

					printf("Output:\n");
					Print1DMatrix(Output, OutDim);

					printf("Weights:\n");
					for(int i = 0; i < InDim; ++i)
					{
						printf("Input %d:\n", i+1);
						Print1DMatrix(Weights[i], OutDim);
					}
				}

				Compare1D(Output, TestOutput, OutDim, Margin);

				Free1D(Input);
				Free1D(Output);

				for(int i = 0; i < InDim; ++i)
				{
					Free1D(Weights[i]);
				}
				free(Weights);

				printf("Fcon Test Complete\n\n");
			}

		// 1.2.1 --- Backward --- //

			void FconBackTest()
			{
				printf("Starting Fcon Backward Test\n\n");

				char Act = ReLu, Outputs = 4;
				double DropP = 0;
				int InDim = 16, OutDim = 32;
				double LearningRate = 0.01;

				double* PrevInput = Init1D(InDim);
				RandomizeArray1D(PrevInput, InDim, 0, 1);

				double* PrevOutput = Init1D(OutDim);
				RandomizeArray1D(PrevOutput, OutDim, 0, 1);

				double* Error = Init1D(OutDim);
				RandomizeArray1D(Error, OutDim, 0, 1);

				double* Output = Init1D(InDim);

				double** Weights = malloc(sizeof(double*) * InDim);
				for(int i = 0; i < InDim; ++i)
				{
					Weights[i] = Init1D(OutDim);
					RandomizeArray1D(Weights[i], OutDim, 0, 5);
				}

				double Params[3] = {Act, DropP, Outputs};

				FconBackCpu(PrevInput, InDim, PrevOutput, OutDim, Error, Output, Weights, Params, LearningRate);

				if(Debug)
				{
					printf("Params:\n");
					if(Act == ReLu)
					{
						printf("Act: Relu, ");
					}
					else if(Act == Sigmoid)
					{
						printf("Act: Sigmoid, ");
					}
					else
					{
						printf("Act: Tanh, ");
					}
					printf("DropP: %.2f\n", DropP);

					printf("PrevInput:\n");
					Print1DMatrix(PrevInput, InDim);

					printf("PrevOutput:\n");
					Print1DMatrix(PrevOutput, OutDim);

					printf("Error:\n");
					Print1DMatrix(Error, OutDim);

					printf("Output:\n");
					Print1DMatrix(Output, InDim);

					printf("Weights:\n");
					for(int i = 0; i < InDim; ++i)
					{
						printf("Input %d:\n", i+1);
						Print1DMatrix(Weights[i], OutDim);
					}
				}

				Free1D(PrevInput);
				Free1D(PrevOutput);
				Free1D(Error);
				Free1D(Output);
				for(int i = 0; i < InDim; ++i)
				{
					Free1D(Weights[i]);
				}
				free(Weights);

				printf("Fcon Backward Test Complete\n\n");
			}

	// 1.3 --- Pool --- //

		// 1.3.1 --- Forward --- //

			void PoolForwTest()
			{
				printf("Starting Pool Test\n\n");

				char Act = 0;
				char WindowSize = 2, Type = MaxPool, Stride = 1;

				double Params[4] = {Act, WindowSize, Type, Stride};

				int NChannels = 3, InDim = 28;

				double Margin = 0.5e-3;

				int InDims[3];
				InDims[0] = NChannels;
				InDims[1] = InDim;
				InDims[2] = InDim;

				int OutDims[3];
				OutDims[0] = InDims[0];
				OutDims[1] = 1 + ( (InDims[1] - WindowSize) / Stride);
				OutDims[2] = OutDims[1];

				FILE* InputDataFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Pool/TestData.txt", "r");
				FILE* OutputDataFile = fopen("CPUCode/Includes/CNN/Tests/TestSource/Layers/PythonResults/Pool/TestOutput.txt", "r");

				double*** Input = Init3D(InDims);							// Input
				for(int channel = 0; channel < InDims[0]; ++channel)
				{
					for(int y = 0; y < InDims[1]; ++y)
					{
						for(int x = 0; x < InDims[2]; ++x)
						{
							fscanf(InputDataFile, "%lf", &Input[channel][y][x]);
						}
					}
				}

				double*** TestOutput = Init3D(OutDims);
				for(int channel = 0; channel < OutDims[0]; ++channel)
				{
					for(int y = 0; y < OutDims[1]; ++y)
					{
						for(int x = 0; x < OutDims[2]; ++x)
						{
							fscanf(OutputDataFile, "%lf", &TestOutput[channel][y][x]);
						}
					}
				}

				double*** Mask = Init3D(InDims);							// Mask ( only needed for training)

				double*** Output = Init3D(OutDims);							// Output

				PoolForwCpu(Input, InDims, Mask, Output, Params);


				if(Debug)
				{
					printf("Params:\n");
					if(Act == ReLu)
					{
						printf("Act: Relu, ");
					}
					else if(Act == Sigmoid)
					{
						printf("Act: Sigmoid, ");
					}
					else
					{
						printf("Act: Tanh, ");
					}
					if(Type == MaxPool)
					{
						printf("Type: MaxPool, ");
					}
					else
					{
						printf("Type: MeanPool, ");
					}
					printf("WindowSize: %d, Stride: %d\n", WindowSize, Stride);

					printf("Input:\n");
					Print3DMatrix(Input, InDims);

					printf("Mask:\n");
					Print3DMatrix(Mask, InDims);

					printf("Output:\n");
					Print3DMatrix(Output, OutDims);
				}

				Compare3D(Output, TestOutput, OutDims, Margin);
				
				Free3D(Input);
				Free3D(Mask);
				Free3D(Output);
				Free3D(TestOutput);
				
				printf("Pool Test Complete\n\n");
			}

		// 1.3.2 --- Backward --- //

			void PoolBackTest()
			{
				printf("Starting Pool Backward Test\n\n");

				char Act = Tanh;
				char WindowSize = 2, Type = MaxPool, Stride = 2;

				double Params[4] = {Act, WindowSize, Type, Stride};

				int NChannels = 3, InDim = 8;

				int InDims[3];
				InDims[0] = NChannels;
				InDims[1] = InDim;
				InDims[2] = InDim;

				int OutDims[3];
				OutDims[0] = InDims[0];
				OutDims[1] = 1 + ( (InDims[1] - WindowSize) / Stride);
				OutDims[2] = OutDims[1];

				double*** PrevOutput = Init3D(OutDims);
				RandomizeArray3D(PrevOutput, OutDims, 0, 3);

				double*** Error = Init3D(OutDims);
				RandomizeArray3D(Error, OutDims, 0, 3);

				double*** Mask = Init3D(InDims);
				for(int channel = 0; channel < OutDims[0]; channel++)
				{
					for(int i = 0; i < OutDims[1]; i++)
					{
						for(int j = 0; j < OutDims[1]; j++)
						{
							int y = round(GenerateRand(0, WindowSize - 1));
							int x = round(GenerateRand(0, WindowSize - 1));

							Mask[channel][(i * WindowSize) + y][(j * WindowSize) + x] = 1;
						}
					}
				}

				double*** Output = Init3D(InDims);
				
				PoolBackCpu(PrevOutput, OutDims, Error, Mask, Output, Params);

				if(Debug)
				{
					printf("Params:\n");
					if(Act == ReLu)
					{
						printf("Act: Relu, ");
					}
					else if(Act == Sigmoid)
					{
						printf("Act: Sigmoid, ");
					}
					else
					{
						printf("Act: Tanh, ");
					}
					if(Type == MaxPool)
					{
						printf("Type: MaxPool, ");
					}
					else
					{
						printf("Type: MeanPool, ");
					}
					printf("WindowSize: %d, Stride: %d\n", WindowSize, Stride);

					printf("PrevOutput:\n");
					Print3DMatrix(PrevOutput, OutDims);

					printf("Error:\n");
					Print3DMatrix(Error, OutDims);

					printf("Mask:\n");
					Print3DMatrix(Mask, InDims);

					printf("Output:\n");
					Print3DMatrix(Output, InDims);
				}
				
				Free3D(PrevOutput);
				Free3D(Error);
				Free3D(Mask);
				Free3D(Output);

				printf("Pool Backward Test Complete\n\n");
			}
