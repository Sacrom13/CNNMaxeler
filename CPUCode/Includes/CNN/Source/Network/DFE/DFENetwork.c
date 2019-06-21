#include "../../../CNN.h"

/*
                File Structure
                
    1 - Global Variables
    	1.1 - Design Parameters
    	1.2 - Network Parameters

    2 - Parameter Definition
    	2.1 - Design Parameters
			2.1.1 - Design Freq
			2.1.2 - LMem Freq

    3 - Write Parameters
		3.1 - Write Layers
		3.2 - Write Params

	4 - DFE Compile
		4.1 - Setup Params
		4.2 - Compile


*/

// TESTHELPER
static double*** BlockForwardCpu(Block Block, double*** Input, char* Flag1D)
{
	// --- Setup for Computation --- //

		double**** LayerOutputs = malloc(sizeof(double***) * (Block.BlockSize + 1));

		LayerOutputs[0] = Init3D(Block.Dims[0]);
		Copy3D(Input, LayerOutputs[0], Block.Dims[0]);

	// --- Go Through each Layer in Block and Compute Output--- //

		for(int Layer = 0; Layer < Block.BlockSize; ++Layer)
		{
			// --- Init Output Volume --- //

				LayerOutputs[Layer + 1] = Init3D(Block.Dims[Layer + 1]);

			// --- Calculate Layer Output --- //

				switch(Block.Layers[Layer])
				{
					case Conv:		// Conv
								ConvForwCpu(LayerOutputs[Layer], Block.Dims[Layer],
											LayerOutputs[Layer + 1],
											Block.Weights[Layer], Block.LayerParams[Layer]);
								break;

					case Pool:		// Pool
								PoolForwCpu(LayerOutputs[Layer], Block.Dims[Layer],
											Block.Weights[Layer][0],
											LayerOutputs[Layer + 1],
											Block.LayerParams[Layer]);
								break;

					case Fcon:		// Fcon

								// Convert Size if Necessary
								if((*Flag1D) == 0)
								{
									int InDim = Block.Dims[Layer][0] * Block.Dims[Layer][1] * Block.Dims[Layer][2];

									double* Aux = Init1D(InDim);
									ConvertTo1D(LayerOutputs[Layer], Aux, Block.Dims[Layer]);

									FconForwCpu(Aux, Block.Dims[Layer][0] * Block.Dims[Layer][1] * Block.Dims[Layer][2],
													LayerOutputs[Layer + 1][0][0], Block.Dims[Layer + 1][2],
													Block.Weights[Layer][0][0], Block.LayerParams[Layer],
													0);

									Free1D(Aux);

									*Flag1D = 1;

								}
								else
								{
									FconForwCpu(LayerOutputs[Layer][0][0], Block.Dims[Layer][0] * Block.Dims[Layer][1] * Block.Dims[Layer][2],
												LayerOutputs[Layer + 1][0][0], Block.Dims[Layer + 1][2],
												Block.Weights[Layer][0][0], Block.LayerParams[Layer],
												0);
								}

								break;
				}

				Free3D(LayerOutputs[Layer]);
		}


		double*** Output = Init3D(Block.Dims[Block.BlockSize]);

		Copy3D(LayerOutputs[Block.BlockSize], Output, Block.Dims[Block.BlockSize]);

	// --- Free --- //

		Free3D(LayerOutputs[Block.BlockSize]);
		free(LayerOutputs);

	return Output;
}
// TEST HELPER

// 1 --- Global Variables --- //

	// 1.1 --- Design Parameters --- //

		static int DesignFreq = DefDesignFreq;					// Frequency the design will run at
		static int LMemFreq = DefLMemFreq;						// LMem Access Frequency

	// 1.2 --- DFE Run Parameters --- //

		static DFEForwParams* FParams;						// Forward Parameters
		static DFEBackParams* BParams;						// Backward Parameters

// 2 --- Parameter Definition --- //

	// 2.1 --- Design Parameters --- //

		// 2.1.1 --- LMem Freq --- //
		
			/*
				Set Large Memory Access Frequency

				Freq - Frequency

				return value - nothing
			*/

			void SetLMemFreq(int Freq)
			{
				LMemFreq = Freq;
			}

		// 2.1.2 --- Design Freq --- //

			/*
				Set Design Memory Access Frequency

				Freq - Frequency

				return value - nothing
			*/

			void SetDesignFreq(int Freq)
			{
				DesignFreq = Freq;
			}

// 3 --- Write Parameters --- //

	// 3.1 --- Write Layers --- //

		/*
			Write Layers of a Block to a File

			Block - Block to Write
			Filename - Name of file to write
			BurstMult - BurstMultiplier for every layer
			Parallelism - Parallelism for every layer
			Flag - forward or backward ( 1 - Forward, 0 - Backward)

			return value - nothing
		*/

		static void WriteLayers(Block Block, char* Filename, int* BurstMult, int* Parallelism, int Flag)
		{
			FILE *fp;

			// --- Change to Correct Directory --- //

				if(chdir("../EngineCode") != 0)
				{
					printf("Change Directory Error.\n");
					exit(DirectoryError);
				}

			// --- Open File --- //

				fp = fopen(Filename, "w+");
				if(fp == NULL)
				{
					printf("File opening Error.\n");
					exit(FileError);
				}

			// --- Write Layer Information --- //

				for(int i = 0; i < Block.BlockSize; ++i)
				{
					fprintf(fp, "%d %d %d ", Block.Layers[i], Parallelism[i], BurstMult[i]);

					switch(Block.Layers[i])
					{
						case Conv:
									// --- Write Params  --- //

										for(int j = 0; j < 5; ++j)
										{
											fprintf(fp, "%.2f ", Block.LayerParams[i][j]);
										}

									// --- Write Input Dims --- //

										fprintf(fp, "%d %d\n", Block.Dims[i][0], Block.Dims[i][1]);
									break;

						case Pool:
									// --- Write Params  --- //

										for(int j = 0; j < 4; ++j)
										{
											fprintf(fp, "%.2f ", Block.LayerParams[i][j]);
										}

									// --- Write Input Dims --- //

										fprintf(fp, "%d %d\n", Block.Dims[i][0], Block.Dims[i][1]);
									break;

						case Fcon:
									// --- Write Params  --- //

										for(int j = 0; j < 1; ++j)
										{
											fprintf(fp, "%.2f ", Block.LayerParams[i][j]);
										}

									// --- Write Input Dims --- //

										fprintf(fp, "%d %d\n", Block.Dims[i][0], Block.Dims[i][2]);
									break;

						case Soft:
									break;
					}
				}

			// --- Write Block Output Dimensions and extra parameters--- //

				fprintf(fp, "0 %d %d %d\n", Block.Dims[Block.BlockSize][0], Block.Dims[Block.BlockSize][2], Flag);

			// --- Close File --- //

				if(fclose(fp) != 0)
				{
					printf("File closing Error");
					exit(FileError);
				}

			// --- Change back to Correct Directory --- //
				
				if(chdir("../CPUCode") != 0)
				{
					printf("Change Directory Error.\n");
					exit(DirectoryError);
				}
		}

	// 3.2 --- Write Params --- //

		/*
			Write Layers of a Block to a File

			Block - Block to Write

			return value - nothing
		*/

		static void WriteDesignParams()
		{
			FILE *fp;

			// --- Change to Correct Directory --- //

				if(chdir("../EngineCode") != 0)
				{
					printf("Change Directory Error.\n");
					exit(DirectoryError);
				}

			// --- Open File --- //

				fp = fopen("params.txt", "w+");
				if(fp == NULL)
				{
					printf("File opening Error.\n");
					exit(FileError);
				}

			// --- Write Params --- //

				fprintf(fp, "%d %d\n", LMemFreq, DesignFreq);

			// --- Close File --- //

				if(fclose(fp) != 0)
				{
					printf("File closing Error");
					exit(FileError);
				}

			// --- Change back to previous Directory --- //

				if(chdir("../CPUCode"))
				{
					printf("Change Directory Error.\n");
					exit(DirectoryError);
				}
		}

// 4 --- DFE Compile --- //

	// 4.1 --- Setup Parameters --- //


		static void SetupForwParams(Network* Net, int Block, int* BurstMult)
		{
			// NCalls

				int **LayerCalls = malloc((Net->Blocks[Block].BlockSize) * sizeof(int*));
				if(LayerCalls == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				for(int Layer = 0; Layer < Net->Blocks[Block].BlockSize; ++Layer)
				{
					LayerCalls[Layer] = malloc(2 * sizeof(int));
					if(LayerCalls[Layer] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					switch(Net->Blocks[Block].Layers[Layer])
					{
						case Conv:
									if(Layer == 0)
									{
										LayerCalls[Layer][0] = 0;
									}
									else
									{
										LayerCalls[Layer][0] = LayerCalls[Layer - 1][1];
									}
									LayerCalls[Layer][1] = LayerCalls[Layer][0] + ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)(BurstMult[Layer] * BurstSizeDataType)));
									break;

						case Pool:
									if(Layer == 0)
									{
										LayerCalls[Layer][0] = 0;
										LayerCalls[Layer][1] = LayerCalls[Layer][0] + ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)(BurstMult[Layer] * BurstSizeDataType)));
									}
									else
									{
										LayerCalls[Layer][0] = LayerCalls[Layer - 1][1] + 2 - ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)(BurstMult[Layer] * BurstSizeDataType)));
										LayerCalls[Layer][1] = LayerCalls[Layer][0] + ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)(BurstMult[Layer] * BurstSizeDataType)));
									}

									break;

						case Fcon:
									if(Layer == 0)
									{
										LayerCalls[Layer][0] = 0;
									}
									else
									{
										//LayerCalls[Layer][0] = LayerCalls[Layer - 1][1];
										if(Net->Blocks[Block].Layers[Layer - 1] == Fcon)
										{
											LayerCalls[Layer][0] = LayerCalls[Layer - 1][1];
										}
										else
										{
											LayerCalls[Layer][0] = LayerCalls[Layer - 1][0] + 1;
										}
									}
									LayerCalls[Layer][1] = LayerCalls[Layer][0] + (int)(ceil(Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].Dims[Layer][1] * Net->Blocks[Block].Dims[Layer][2] / (float) (BurstMult[Layer] * BurstSizeDataType)) * ceil(Net->Blocks[Block].Dims[Layer + 1][2] / (float)(BurstMult[Layer] * BurstSizeDataType)));
									break;
					}
				}
				FParams[Block].NCalls = LayerCalls[Net->Blocks[Block].BlockSize - 1][1];

			// Allocations

				// First Outputs
				FParams[Block].FirstOutputs = malloc(FParams[Block].NCalls * sizeof(uint32_t*));
				if(FParams[Block].FirstOutputs == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				// Mem Control
				FParams[Block].MemControl = malloc(FParams[Block].NCalls * sizeof(uint32_t*));
				if(FParams[Block].MemControl == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				// DFEWeights
				FParams[Block].DFEWeights = malloc(FParams[Block].NCalls * sizeof(double*));
				if(FParams[Block].DFEWeights == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				for(int CurrentCall = 0; CurrentCall < (int) FParams[Block].NCalls; ++CurrentCall)
				{
					// First Outputs
					FParams[Block].FirstOutputs[CurrentCall] = calloc(Net->Blocks[Block].BlockSize, sizeof(uint32_t));
					if(FParams[Block].FirstOutputs[CurrentCall] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					// Mem Control
					FParams[Block].MemControl[CurrentCall] = calloc(Net->Blocks[Block].BlockSize, sizeof(uint32_t));
					if(FParams[Block].MemControl[CurrentCall] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					// DFEWeights
					FParams[Block].DFEWeights[CurrentCall] = calloc(pow(2, 16), sizeof(double));
					if(FParams[Block].DFEWeights[CurrentCall] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}
				}

			// Setup

				int* pos = calloc(FParams[Block].NCalls, sizeof(int));
				if(pos == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				int CurrentKernel;
				int OutputSize;

				for(int Layer = 0; Layer < Net->Blocks[Block].BlockSize; ++Layer)
				{
					switch(Net->Blocks[Block].Layers[Layer])
					{
						case Conv:
									// Setup Variables for this Kernel
									CurrentKernel = 0;

									OutputSize = BurstMult[Layer] * BurstSizeDataType;

									for(int CurrentCall = LayerCalls[Layer][0]; CurrentCall < LayerCalls[Layer][1]; ++CurrentCall)
									{
										//printf("\33[2KSetting Block %d, Layer %d, (%d/%d)\r", Block, Layer, CurrentCall - LayerCalls[Layer][0], LayerCalls[Layer][1] - LayerCalls[Layer][0]);

										// First Outputs
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											FParams[Block].FirstOutputs[CurrentCall][Layer] = 0;
										}
										else
										{
											FParams[Block].FirstOutputs[CurrentCall][Layer] = FParams[Block].FirstOutputs[CurrentCall - 1][Layer] + OutputSize;
											if((int)FParams[Block].FirstOutputs[CurrentCall][Layer] > Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
											{
												FParams[Block].FirstOutputs[CurrentCall][Layer] = FParams[Block].FirstOutputs[CurrentCall][Layer] % (Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2]);
											}
										}

										// Weights
										for(int Kernel = 0; Kernel < 2; ++Kernel)
										{
											for(int Channel = 0; Channel < Net->Blocks[Block].Dims[Layer][0]; ++Channel)
											{
												for(int y = 0; y < Net->Blocks[Block].LayerParams[Layer][2]; ++y)
												{
													for(int x = 0; x < Net->Blocks[Block].LayerParams[Layer][2]; ++x)
													{
														if(CurrentKernel + Kernel != Net->Blocks[Block].Dims[Layer + 1][0])
														{
															FParams[Block].DFEWeights[CurrentCall][pos[CurrentCall]] = Net->Blocks[Block].Weights[Layer][CurrentKernel + Kernel][Channel][y][x];
														}
														else
														{
															FParams[Block].DFEWeights[CurrentCall][pos[CurrentCall]] = 0;
														}
														++pos[CurrentCall];
													}
												}
											}
										}
										if((int)FParams[Block].FirstOutputs[CurrentCall][Layer] + OutputSize > Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
										{
											CurrentKernel++;
										}

										// Mem Control
										FParams[Block].MemControl[CurrentCall][Layer] = 1 + CurrentCall - LayerCalls[Layer][0];
									}

									break;
						case Pool:

									OutputSize = BurstMult[Layer] * BurstSizeDataType;
									for(int CurrentCall = LayerCalls[Layer][0]; CurrentCall < LayerCalls[Layer][1]; ++CurrentCall)
									{
										//printf("\33[2KSetting Block %d, Layer %d, (%d/%d)\r", Block, Layer, CurrentCall - LayerCalls[Layer][0], LayerCalls[Layer][1] - LayerCalls[Layer][0]);

										// First Outputs
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											FParams[Block].FirstOutputs[CurrentCall][Layer] = 0;
										}
										else
										{
											FParams[Block].FirstOutputs[CurrentCall][Layer] = FParams[Block].FirstOutputs[CurrentCall - 1][Layer] + OutputSize;
										}

										// Mem Control
										FParams[Block].MemControl[CurrentCall][Layer] = 1 + CurrentCall - LayerCalls[Layer][0];
									}

									break;

						case Fcon:
									OutputSize = BurstMult[Layer] * BurstSizeDataType;
									for(int CurrentCall = LayerCalls[Layer][0]; CurrentCall < LayerCalls[Layer][1]; ++CurrentCall)
									{
										//printf("\33[2KSetting Block %d, Layer %d, (%d/%d)\r", Block, Layer, CurrentCall - LayerCalls[Layer][0], LayerCalls[Layer][1] - LayerCalls[Layer][0]);

										// First Outputs ( Used as Input Mem Control in this layer)
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											FParams[Block].FirstOutputs[CurrentCall][Layer] = 0;
										}
										else
										{
											FParams[Block].FirstOutputs[CurrentCall][Layer] = FParams[Block].FirstOutputs[CurrentCall - 1][Layer] + 1;
											if((int)(BurstSizeDataType * BurstMult[Layer] * FParams[Block].FirstOutputs[CurrentCall][Layer]) >= Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].Dims[Layer][1] * Net->Blocks[Block].Dims[Layer][2])
											{
												FParams[Block].FirstOutputs[CurrentCall][Layer] = 0;
											}
										}

										// Output Mem Control
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											FParams[Block].MemControl[CurrentCall][Layer] = 1;
										}
										else
										{
											if(FParams[Block].FirstOutputs[CurrentCall][Layer] == 0)
											{
												FParams[Block].MemControl[CurrentCall][Layer] = FParams[Block].MemControl[CurrentCall - 1][Layer] + 1;
											}
											else
											{
												FParams[Block].MemControl[CurrentCall][Layer] = FParams[Block].MemControl[CurrentCall - 1][Layer];
											}
										}

										// Weights
										for(int i = 0; i < BurstSizeDataType * BurstMult[Layer]; ++i)
										{
											for(int j = 0; j < OutputSize; ++j)
											{
												if((int)(FParams[Block].FirstOutputs[CurrentCall][Layer] * BurstSizeDataType * BurstMult[Layer] + i) < Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].Dims[Layer][1] * Net->Blocks[Block].Dims[Layer][2])
												{
													if((int)(BurstMult[Layer] * BurstSizeDataType * (FParams[Block].MemControl[CurrentCall][Layer] - 1) + j) < Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
													{
														FParams[Block].DFEWeights[CurrentCall][pos[CurrentCall]] = Net->Blocks[Block].Weights[Layer][0][0][(FParams[Block].FirstOutputs[CurrentCall][Layer] * BurstSizeDataType * BurstMult[Layer] + i)][BurstMult[Layer] * BurstSizeDataType * (FParams[Block].MemControl[CurrentCall][Layer] - 1) + j];
													}
													else
													{
														FParams[Block].DFEWeights[CurrentCall][pos[CurrentCall]] = 0;
													}
												}
												else
												{
													FParams[Block].DFEWeights[CurrentCall][pos[CurrentCall]] = 0;
												}
												++pos[CurrentCall];
											}
										}
									}

									break;
					}
				}
				//printf("\33[2K");
				for(int Layer = 0; Layer < Net->Blocks[Block].BlockSize; ++Layer)
				{
					free(LayerCalls[Layer]);
				}
				free(LayerCalls);
				free(pos);
		}

	// 4.2 --- Compile to Hardware --- //

		void DFECompile(Network* Net, int** BurstMult, int** ForwParallelism, int** BackParallelism)
		{
			printf("Compiling Network to Hardware!\n");

			// --- Check if Parameters are correct --- //

				for(int Block = 0; Block < Net->TotalBlocks; ++Block)
				{
					int WeightDims = 0;
					for(int Layer = 0; Layer < Net->Blocks[Block].BlockSize; ++Layer)
					{
						if(BurstMult[Block][Layer] < 1)
						{
							printf("Cannot compile to DFE.\n");
							printf("Layer %d in Block %d has invalid parameters.", Layer + 1, Block + 1);
							printf("BurstMult has to be >= 1.");
							exit(DesignError);
						}
						if(ForwParallelism[Block][Layer] < 1)
						{
							printf("Cannot compile to DFE.\n");
							printf("Layer %d in Block %d has invalid parameters.", Layer + 1, Block + 1);
							printf("LayerParallelism has to be >= 1.");
							exit(DesignError);
						}
						if(BackParallelism[Block][Layer] < 1)
						{
							printf("Cannot compile to DFE.\n");
							printf("Layer %d in Block %d has invalid parameters.", Layer + 1, Block + 1);
							printf("BackParallelism has to be >= 1.");
							exit(DesignError);
						}

						switch(Net->Blocks[Block].Layers[Layer])
						{
							case Conv:
										// Check if Output Size does not exceed 2 channels at once

											if(BurstMult[Block][Layer] * BurstSizeDataType >= 2 * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
											{
												printf("Cannot compile to DFE.\n");
												printf("Layer %d in Block %d has Output Dims %dx%d\n", Layer + 1, Block + 1, Net->Blocks[Block].Dims[Layer + 1][1], Net->Blocks[Block].Dims[Layer + 1][2]);
												printf("BurstMult*BurstSize = %dx%d = %d, which has to be less than 2*%d*%d = %d\n", BurstMult[Block][Layer], BurstSizeDataType, BurstMult[Block][Layer] * BurstSizeDataType, Net->Blocks[Block].Dims[Layer + 1][1], Net->Blocks[Block].Dims[Layer + 1][2], 2 * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2]);
												exit(DesignError);
											}

										// Check if Weights fit in FMem

											WeightDims += 2 * Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].LayerParams[Layer][2] * Net->Blocks[Block].LayerParams[Layer][2];
											if(WeightDims > pow(2, 16))
											{
												printf("Cannot compile to DFE.\n");
												printf("Layer %d in Block %d results in Weight Dimensions beeing > 65536.\n", Layer + 1, Block + 1);
												printf("Weights used by this layer: 2 * InChannels(%d) * KernelSize*KernelSize (%dx%d) = %d.\n", Net->Blocks[Block].Dims[Layer][0], (int)Net->Blocks[Block].LayerParams[Layer][2], (int)Net->Blocks[Block].LayerParams[Layer][2], (int)(Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].LayerParams[Layer][2] * Net->Blocks[Block].LayerParams[Layer][2]));
												printf("Try lowering forward parallelism for layers in this block or lowering burstmult.");
												exit(DesignError);
											}

										// Check if Parallelism is allowed

											if(ForwParallelism[Block][Layer] > Net->Blocks[Block].Dims[Layer][0]/2)
											{
												printf("Cannot compile to DFE.\n");
												printf("Layer %d in Block %d has invalid forward parallelism\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
												printf("Forward parallelism (%d) cannot be > InChannels/2 = %d\n", ForwParallelism[Block][Layer], Net->Blocks[Block].Dims[Layer][0]/2);
												exit(DesignError);
											}
											if((Net->Blocks[Block].Dims[Layer][0] % ForwParallelism[Block][Layer]) != 0)
											{
												printf("Cannot compile to DFE.\n");
												printf("Layer %d in Block %d has invalid forward parallelism\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
												printf("InChannels(%d)/Forward parallelism(%d) is not an even number.\n", Net->Blocks[Block].Dims[Layer][0], ForwParallelism[Block][Layer]);
												exit(DesignError);
											}

										break;
							case Fcon:
										// Check if Weights fit in FMem

											WeightDims += pow(BurstMult[Block][Layer] * BurstSizeDataType, 2);
											if(WeightDims > pow(2, 16))
											{
												printf("Cannot Compile to DFE.\n");
												printf("Layer %d in Block %d results in Weight Dimensions beeing > 65536.\n", Layer + 1, Block + 1);
												printf("Weights used by this layer: (BurstMult*BurstSizeDataType)^2 = (%d*%d)^2 = %d.\n", BurstMult[Block][Layer], BurstSizeDataType, (int)pow(BurstMult[Block][Layer] * BurstSizeDataType, 2));
												exit(DesignError);
											}

										// Check if Parallelism is allowed

											if(ForwParallelism[Block][Layer] > BurstMult[Block][Layer] * BurstSizeDataType/2)
											{
												printf("Layer %d in Block %d.\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
												printf("Forward parallelism (%d), cannot be greater than 12 * BurstMult(%d)\n", ForwParallelism[Block][Layer], BurstMult[Block][Layer] * BurstSizeDataType/2);
												exit(DesignError);
											}

										break;
						}
					}
				}

			// --- Compile Blocks --- //

				printf("Compiling Blocks\n");

				WriteDesignParams();


				WriteLayers(Net->Blocks[0], "layers0.txt", BurstMult[0], ForwParallelism[0], 1);
				WriteLayers(Net->Blocks[1], "layers1.txt", BurstMult[1], ForwParallelism[1], 1);

				printf("Blocks Compiled\n");

			// --- Setup Parameters --- //

				FParams = malloc(Net->TotalBlocks * sizeof(DFEForwParams));
				if(FParams == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}
				BParams = malloc(Net->TotalBlocks * sizeof(DFEForwParams));
				if(BParams == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}
				for(int i = 0; i < Net->TotalBlocks; ++i)
				{
					FParams[i].Parallelism = malloc(Net->Blocks[i].BlockSize * sizeof(int));
					if(FParams[i].Parallelism == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					FParams[i].BurstMult = malloc(Net->Blocks[i].BlockSize * sizeof(int));
					if(FParams[i].BurstMult == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}
				}

				// First Block

					// Input Offset + Parallelism + BurstSize
					FParams[0].InputOffset = 0;
					for(int Layer = 0; Layer < Net->Blocks[0].BlockSize; ++Layer)
					{
						FParams[0].Parallelism[Layer] = ForwParallelism[0][Layer];
						FParams[0].BurstMult[Layer] = BurstMult[0][Layer];
					}

					// Remaining Params
					SetupForwParams(Net, 0, BurstMult[0]);

				// Remaining Blocks

					for(int Block = 1; Block < Net->TotalBlocks; ++Block)
					{
						// InputOffset
						FParams[Block].InputOffset = FParams[Block - 1].InputOffset;
						for(int Layer = 0; Layer < Net->Blocks[Block - 1].BlockSize; ++Layer)
						{
							int LayerMemSize = Net->Blocks[Block - 1].Dims[Layer][0] * Net->Blocks[Block - 1].Dims[Layer][1] * Net->Blocks[Block - 1].Dims[Layer][2];
							if(LayerMemSize % (int)(BurstMult[Block - 1][Layer] * BurstSizeDataType) != 0)
							{
								LayerMemSize += ((BurstMult[Block - 1][Layer] * BurstSizeDataType) - (LayerMemSize % (int)(BurstMult[Block - 1][Layer] * BurstSizeDataType)));
							}
							FParams[Block].InputOffset += (LayerMemSize*sizeof(double));

							if(Net->Blocks[Block - 1].Layers[Layer] == Pool)
							{
								FParams[Block].InputOffset += (LayerMemSize*sizeof(double));
							}

							// Parallelism + BurstSize
							FParams[Block].Parallelism[Layer] = ForwParallelism[Block][Layer];
							FParams[Block].BurstMult[Layer] = BurstMult[Block][Layer];
						}
						// Remaining Params
						SetupForwParams(Net, Block, BurstMult[Block]);
					}

			printf("Network sucessfully compiled!");
		}

	static void BlockForwardDFE(int Block)
	{
		// --------------------------------------------- //
		// ----------     	Basic Static	   ----------//
		// --------------------------------------------- //

			// --------------------------------------------- //
			// ---------- 		Computation    	   ----------//
			// --------------------------------------------- //


			for(int CurrentCall = 0; CurrentCall < (int)FParams[Block].NCalls; ++CurrentCall)
			{
				printf("Running Call %d/%d.\n", CurrentCall + 1, FParams[Block].NCalls);

				// Running Blocking version.
				Block0_RunForward(
						FParams[Block].InputOffset,
						FParams[Block].FirstOutputs[CurrentCall],
						FParams[Block].MemControl[CurrentCall],
						FParams[Block].DFEWeights[CurrentCall]);

				// To run NonBlock version
					//max_nowait(MovingAverageSimple_RunForward_nonblock(
					//	FParams[Block].InputOffset,
					//	FParams[Block].FirstOutputs[CurrentCall],
					//	FParams[Block].MemControl[CurrentCall],
					//	FParams[Block].DFEWeights[CurrentCall]));

				//	Note: max_nowait can be replaced with max_wait() for control.
				
			}
			printf("\n");

		// --------------------------------------------- //
		// ----------     Advanced Static      ----------//
		// --------------------------------------------- //

		// Not working. Problem with max_load
		/*
			// Init Max File
			printf("Loading Max File!\n");
			max_file_t *File = MovingAverageSimple_init();
			max_engine_t *Engine = max_load(File, '*');
			printf("Max File Loaded!\n");

			// --------------------------------------------- //
			// ---------- 		Computation    	   ----------//
			// --------------------------------------------- //


			for(int CurrentCall = 0; CurrentCall < (int)Block.FParams.NCalls; ++CurrentCall)
			{
				MovingAverageSimple_RunForward_actions_t Actions;
				Actions.param_FirstOutputs = FParams[Block].FirstOutputs[CurrentCall];
				Actions.param_InputOffset = FParams[Block].InputOffset;
				Actions.param_MemControl = FParams[Block].MemControl[CurrentCall];
				Actions.param_Weights = FParams[Block].DFEWeights[CurrentCall];


				printf("Running Call %d/%d.\n", CurrentCall + 1, FParams.NCalls);
				MovingAverageSimple_RunForward_run(Engine, &Actions);
			}
			printf("\n");*/
	}

	double* CNNForwardDFE(Network Net, double*** Input)
	{
		// Setup Input

		int InDims1D = Net.Blocks[0].Dims[0][0] * Net.Blocks[0].Dims[0][1] * Net.Blocks[0].Dims[0][2];

		if(InDims1D % (FParams[0].BurstMult[0] * BurstSizeDataType) != 0)
		{
			InDims1D += ((FParams[0].BurstMult[0] * BurstSizeDataType) - (InDims1D % (FParams[0].BurstMult[0] * BurstSizeDataType)));
		}
		double* Input1D = Init1D(InDims1D);
		ConvertTo1D(Input, Input1D, Net.Blocks[0].Dims[0]);

		// Write Input to Memory

			printf("Writing to DFE\n");

			Block0_MemWrite(InDims1D, 0, Input1D);

		// DFE Computation

			printf("Running DFE\n");

			StartTiming();
			for(int Block = 0; Block < 1; ++Block)
			{
				BlockForwardDFE(Block);
			}
			printf("DFE Finished. Time Taken = %.2f milliseconds\n", StopTiming()/1000);

		// Read Output from Mem

			printf("Reading from DFE\n");

			int OutputStart = 0;
			for(int i = 0; i < Net.Blocks[0].BlockSize; ++i)
			{
				int DimAux = Net.Blocks[0].Dims[i][0] * Net.Blocks[0].Dims[i][1] * Net.Blocks[0].Dims[i][2];
				if((DimAux % (FParams[0].BurstMult[i] * BurstSizeDataType)) != 0)
				{
					DimAux += ((FParams[0].BurstMult[i] * BurstSizeDataType) - (DimAux % (FParams[0].BurstMult[i] * BurstSizeDataType)));
				}

				OutputStart += DimAux;

				if(Net.Blocks[0].Layers[i] == Pool)
				{
					OutputStart += DimAux;
				}
			}

			int OutDims1D;
			OutDims1D = Net.Blocks[0].Dims[Net.Blocks[0].BlockSize][0] * Net.Blocks[0].Dims[Net.Blocks[0].BlockSize][1] * Net.Blocks[0].Dims[Net.Blocks[0].BlockSize][2];
			if(OutDims1D % (FParams[0].BurstMult[Net.Blocks[0].BlockSize - 1] * BurstSizeDataType) != 0)
			{
				OutDims1D += ((FParams[0].BurstMult[Net.Blocks[0].BlockSize - 1] * BurstSizeDataType) - (OutDims1D % (FParams[0].BurstMult[Net.Blocks[0].BlockSize - 1] * BurstSizeDataType)));
			}

			double* Output = Init1D(OutDims1D);
			Block0_MemRead(OutDims1D,
										OutputStart,
										Output);

			// Print1DMatrix(Output, OutDims1D);

		// Run CPU

			char* aux = malloc(sizeof(char));
			*aux = 0;

			printf("Running CPU!\n");
			StartTiming();
			double*** TestOutput = BlockForwardCpu(Net.Blocks[0], Input, aux);
			printf("CPU Finished. Time Taken = %.2f milliseconds\n", StopTiming()/1000);

		// Check if Correct

			double* TestOutput1D = Init1D(OutDims1D);

			ConvertTo1D(TestOutput, TestOutput1D, Net.Blocks[0].Dims[Net.Blocks[0].BlockSize]);

			// Print1DMatrix(TestOutput1D, OutDims1D);

			double Margin = .5e-2;
			Compare1D(Output, TestOutput1D, OutDims1D, Margin);

		return NULL;
	}
