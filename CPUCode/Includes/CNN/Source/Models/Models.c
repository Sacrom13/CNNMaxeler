#include "../../CNN.h"

/*
        	File Structure

	1 - Global Variables 

	2 - Model Construction
		2.1 - Init
		2.2 - Free
		2.3 - Edit Params
			2.3.1 - BatchSize
			2.3.2 - LearningRate
			2.3.3 - Momentum
			2.3.4 - Error Func
			2.3.5 - BurstMult
		2.4 - AddBlock
		2.5 - AddLayers
			2.5.1 - Conv
			2.5.2 - Pool
			2.5.3 - Fcon
			2.5.4 - Acti
			2.5.5 - Drop

	3 - Pre Defined Models
		3.1 - AlexNet
		3.2 - VGG16

	4 - Print

	5 - Save

	6 - Load
*/

// 1 --- Global Variables --- //

	static Network* CurrentNet;														// Current Network to Add Layers to

// 1 --- Network Construction --- //

	// 2.1 --- Init --- //

		/*
			Initialize Network

			Net - Net to init
			InputDims - Dimensions of Input Volume for the Network
			
			return value - nothing
		*/

		void InitCNN(Network* Net, int* InputDims)
		{
			if(Net == NULL)
			{
				printf("Allocate Network before calling InitCNN().\n");
				exit(MemoryError);
			}

			// --- Init Params --- //

				Net->BatchSize = DefBatchSize;
				Net->LearningRate = DefLearningRate;
				Net->Momentum = DefMomentum;
				Net->EFunc = DefEFunc;

			// --- Init first Block --- //

				Net->TotalBlocks = -1;

				Net->Blocks = malloc(1 * (sizeof(Block)));
				if(Net->Blocks == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				// --- Allocate Dimensions --- //

					Net->Blocks[0].Dims = malloc(sizeof(int*));
					if(Net->Blocks[0].Dims == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					Net->Blocks[0].Dims[0] = calloc(3, sizeof(int));
					if(Net->Blocks[0].Dims[0] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					for(int i = 0; i < 3; ++i)
					{
						Net->Blocks[0].Dims[0][i] = InputDims[i];
					}
		}

	// 2.2 --- Free --- //

		/*
			Free Network

			Net - Network to be freed

			return value - nothing
		*/

		void FreeCNN(Network* Net)
		{
			for(int i = 0; i < Net->TotalBlocks; ++i)
			{
				// --- Free Parallelism --- //

					free(Net->Blocks[i].FParams.Parallelism);

				// --- Free Weights + Params --- //

					for(int j = 0; j < Net->Blocks[i].BlockSize; ++j)
					{
						if(Net->Blocks[i].Layers[j] == Conv)
						{
							for(int k = 0; k < Net->Blocks[i].LayerParams[j][1]; ++k)
							{
								Free3D(Net->Blocks[i].Weights[j][k]);
							}
							free(Net->Blocks[i].Weights[j]);

							Free1D(Net->Blocks[i].LayerParams[j]);
						}
						else if(Net->Blocks[i].Layers[j] == Pool)
						{
							Free3D(Net->Blocks[i].Weights[j][0]);
							free(Net->Blocks[i].Weights[j]);

							Free1D(Net->Blocks[i].LayerParams[j]);
						}
						else if(Net->Blocks[i].Layers[j] == Fcon)
						{
							int InputSize = Net->Blocks[i].Dims[j][0] * Net->Blocks[i].Dims[j][1] * Net->Blocks[i].Dims[j][2];
							for(int k = 0; k < InputSize; ++k)
							{
								Free1D(Net->Blocks[i].Weights[j][0][0][k]);
							}
							free(Net->Blocks[i].Weights[j][0][0]);
							free(Net->Blocks[i].Weights[j][0]);
							free(Net->Blocks[i].Weights[j]);

							Free1D(Net->Blocks[i].LayerParams[j]);
						}
					}
					free(Net->Blocks[i].Weights);

				// --- Free DFE Params --- //

					for(int j = 0; j < (int) Net->Blocks[i].FParams.NCalls; ++j)
					{
						free(Net->Blocks[i].FParams.FirstOutputs[j]);
						free(Net->Blocks[i].FParams.MemControl[j]);

						free(Net->Blocks[i].FParams.DFEWeights[j]);
					}

					free(Net->Blocks[i].FParams.FirstOutputs);
					free(Net->Blocks[i].FParams.MemControl);

					free(Net->Blocks[i].FParams.DFEWeights);

				// --- Free Dims --- //

					for(int j = 0; j < Net->Blocks[i].BlockSize + 1; ++j)
					{
						free(Net->Blocks[i].Dims[j]);
					}
					free(Net->Blocks[i].Dims);

				// --- Free Params --- //

					free(Net->Blocks[i].LayerParams);

				// --- Free Layers --- //

					free(Net->Blocks[i].Layers);
			}

			// --- Free Blocks --- //

				free(Net->Blocks);
		}

	// 2.3 --- Edit Parameters --- //

		// 2.3.1 --- Batch Size --- //

			/*
				Set Batch Size

				Net - Network to consider
				Efunc - Batch Size

				return value - nothing
			*/

			void SetBatchSize(Network* Net, int Bs)
			{
				Net->BatchSize = Bs;
			}

		// 2.3.2 --- Learning Rate --- //

			/*
				Set Learning Rate

				Net - Network to consider
				Efunc - Learning Rate

				return value - nothing
			*/

			void SetLearningRate(Network* Net, double Lr)
			{
				Net->LearningRate = Lr;
			}

		// 2.3.3 --- Momentum --- //

			/*
				Set Momentum

				Net - Network to consider
				Mom - Momentum

				return value - nothing
			*/

			void SetMomentum(Network* Net, double Mom)
			{
				Net->Momentum = Mom;
			}

		// 2.3.4 --- EFunc --- //

			/*
				Set Error Func

				Net - Network to consider
				Efunc - Error Function

				return value - nothing
			*/

			void SetErrorFunc(Network* Net, char Func)
			{
				Net->EFunc = Func;
			}

		// 2.3.5 --- BurstMult --- //

			/*
				Sets BurstMultiplier

				Net - Network to consider
				Block - Block to consider
				BM - Burst Multiplier

				return value - nothing
			*/

			void SetBurstMult(Network* Net, int Block, int BM)
			{
				// Calculate new Output Size
				int OutputSize = BM * BurstSizeDataType;

				// Check if BurstMult is valid
				if(BM < 1)
				{
					printf("Cannot set BurstSize.\n");
					printf("BurstMult has greater than or equal to 1.");
				}

				int WeightDims = 0;
				char FconFlag = 0;
				for(int Layer = 0; Layer < Net->Blocks[Block].BlockSize; ++Layer)
				{
					switch(Net->Blocks[Block].Layers[Layer])
					{
						case Conv:
									// Check if Output Size does not exceed 2 channels at once
									if(BM * BurstSizeDataType >= 2 * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
									{
										printf("Cannot set BurstSize.\n");
										printf("Layer %d(Conv) in Block %d has Output Dims %dx%d\n", Layer + 1, Block + 1, Net->Blocks[Block].Dims[Layer + 1][1], Net->Blocks[Block].Dims[Layer + 1][1]);
										printf("BurstMult*BurstSize = %dx%d =  %d, which has to be less than 2*%d*%d = %d\n", BM, BurstSizeDataType, BM * BurstSizeDataType, Net->Blocks[Block].Dims[Layer + 1][1], Net->Blocks[Block].Dims[Layer + 1][1], 2 * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][1]);
										exit(CNNConstructionError);
									}

									// Check if Weights fit in FMem
									WeightDims = 2 * Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].LayerParams[Layer][2] * Net->Blocks[Block].LayerParams[Layer][2];
									if(WeightDims > pow(2, 16))
									{
										printf("Layer %d(Conv) in Block %d has invalid Kernel Dimensions\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
										printf("2 * InChannels(%d) * KernelSize*KernelSize (%dx%d) = %d. This cannot be greater than 65536\n", Net->Blocks[Block].Dims[Layer][0], (int)Net->Blocks[Block].LayerParams[Layer][2], (int)Net->Blocks[Block].LayerParams[Layer][2], (int)(Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].LayerParams[Layer][2] * Net->Blocks[Block].LayerParams[Layer][2]));
										exit(CNNConstructionError);
									}
									break;
						case Fcon:
									// If Bm > 8 Weights don't fit in FMem.
										if(BM > 8)
										{
											printf("Cannot set BurstSize.\n");
											printf("Layer %d(Fcon) in Block %d.\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
											printf("Blocks with Fcon Layers must have BurstMult < 7\n");
											exit(CNNConstructionError);
										}

									// Check if Weights fit in FMem
										if(FconFlag == 0)
										{
											WeightDims += pow(BM * BurstSizeDataType, 2);
											if(WeightDims > pow(2, 16))
											{
												printf("Layer %d(Fcon) in Block %d has exceeded Weight Dimensions\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
												printf("TotalWeightDims = %d, which cannot be greater than 65536.\n", WeightDims);
												printf("Try lowering BurstSize using SetBurstMult()");
												exit(CNNConstructionError);
											}
											FconFlag = 1;
										}
										else
										{
											WeightDims = 2 * pow(BM * BurstSizeDataType, 2);
											if(WeightDims > pow(2, 16))
											{
												printf("Layer %d(Fcon) in Block %d has exceeded Weight Dimensions\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
												printf("TotalWeightDims = %d, which cannot be greater than 65536.\n", WeightDims);
												printf("Try lowering BurstSize using SetBurstMult()");
												exit(CNNConstructionError);
											}
										}

									// Check if Parallelism is allowed
										if(Net->Blocks[Block].FParams.Parallelism[Layer] > OutputSize/2)
										{
											printf("Fcon Layer %d in Block %d.\n", Net->Blocks[Block].Layers[Layer] + 1, Block + 1);
											printf("Parallelism (%d), cannot be greater than BurstMult*24/2(%d)\n", Net->Blocks[Block].FParams.Parallelism[Layer], OutputSize/2);
											exit(CNNConstructionError);
										}

									break;
					}
				}

				// Free previous Params
				if(Net->Blocks[Block].FParams.NCalls > 0)
				{
					for(int CurrentCall = 0; CurrentCall < (int) Net->Blocks[Block].FParams.NCalls; ++CurrentCall)
					{
						free(Net->Blocks[Block].FParams.FirstOutputs[CurrentCall]);
						free(Net->Blocks[Block].FParams.MemControl[CurrentCall]);;
						free(Net->Blocks[Block].FParams.DFEWeights[CurrentCall]);
					}

					free(Net->Blocks[Block].FParams.FirstOutputs);
					free(Net->Blocks[Block].FParams.MemControl);
					free(Net->Blocks[Block].FParams.DFEWeights);
				}

				// Set Burst Multiplier
				Net->Blocks[Block].FParams.BurstMult = BM;

				// --- Calculate Input Offset --- //

				Net->Blocks[0].FParams.InputOffset = 0;
				for(int i = 0; i < Net->TotalBlocks - 1; ++i)
				{
					Net->Blocks[i + 1].FParams.InputOffset = Net->Blocks[i].FParams.InputOffset;
					for(int Layer = 0; Layer < Net->Blocks[i].BlockSize; ++Layer)
					{
						int LayerMemSize = Net->Blocks[i].Dims[Layer][0] * Net->Blocks[i].Dims[Layer][1] * Net->Blocks[i].Dims[Layer][2];
						if(LayerMemSize % OutputSize != 0)
						{
							LayerMemSize += (OutputSize - (LayerMemSize % OutputSize));
						}
						Net->Blocks[i + 1].FParams.InputOffset += (LayerMemSize*sizeof(double));
					}
				}

				// --- Find out NCalls --- //

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
										LayerCalls[Layer][1] = LayerCalls[Layer][0] + ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)OutputSize));
										break;

							case Pool:
										if(Layer == 0)
										{
											LayerCalls[Layer][0] = 0;
											LayerCalls[Layer][1] = LayerCalls[Layer][0] + ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)OutputSize));
										}
										else
										{
											LayerCalls[Layer][0] = LayerCalls[Layer - 1][1] + 2 - ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)OutputSize));
											LayerCalls[Layer][1] = LayerCalls[Layer][0] + ((int)ceil(Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2] / (float)OutputSize));
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
										LayerCalls[Layer][1] = LayerCalls[Layer][0] + (int)(ceil(Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].Dims[Layer][1] * Net->Blocks[Block].Dims[Layer][2] / (float) OutputSize) * ceil(Net->Blocks[Block].Dims[Layer + 1][2] / (float)OutputSize));
										break;
						}
					}
					Net->Blocks[Block].FParams.NCalls = LayerCalls[Net->Blocks[Block].BlockSize - 1][1];

				// Allocations

					// First Outputs
					Net->Blocks[Block].FParams.FirstOutputs = malloc(Net->Blocks[Block].FParams.NCalls * sizeof(uint32_t*));
					if(Net->Blocks[Block].FParams.FirstOutputs == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					// Mem Control
					Net->Blocks[Block].FParams.MemControl = malloc(Net->Blocks[Block].FParams.NCalls * sizeof(uint32_t*));
					if(Net->Blocks[Block].FParams.MemControl == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					// DFEWeights
					Net->Blocks[Block].FParams.DFEWeights = malloc(Net->Blocks[Block].FParams.NCalls * sizeof(double*));
					if(Net->Blocks[Block].FParams.DFEWeights == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					for(int CurrentCall = 0; CurrentCall < (int) Net->Blocks[Block].FParams.NCalls; ++CurrentCall)
					{
						// First Outputs
						Net->Blocks[Block].FParams.FirstOutputs[CurrentCall] = calloc(Net->Blocks[Block].BlockSize, sizeof(uint32_t));
						if(Net->Blocks[Block].FParams.FirstOutputs[CurrentCall] == NULL)
						{
							printf("Memory Allocation Error.\n");
							exit(MemoryError);
						}

						// Mem Control
						Net->Blocks[Block].FParams.MemControl[CurrentCall] = calloc(Net->Blocks[Block].BlockSize, sizeof(uint32_t));
						if(Net->Blocks[Block].FParams.MemControl[CurrentCall] == NULL)
						{
							printf("Memory Allocation Error.\n");
							exit(MemoryError);
						}

						// DFEWeights
						Net->Blocks[Block].FParams.DFEWeights[CurrentCall] = calloc(pow(2, 16), sizeof(double));
						if(Net->Blocks[Block].FParams.DFEWeights[CurrentCall] == NULL)
						{
							printf("Memory Allocation Error.\n");
							exit(MemoryError);
						}
					}

				int* pos = calloc(Net->Blocks[Block].FParams.NCalls, sizeof(int));
				if(pos == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

				int CurrentKernel;

				for(int Layer = 0; Layer < Net->Blocks[Block].BlockSize; ++Layer)
				{
					switch(Net->Blocks[Block].Layers[Layer])
					{
						case Conv:
									// Setup Variables for this Kernel
									CurrentKernel = 0;

									for(int CurrentCall = LayerCalls[Layer][0]; CurrentCall < LayerCalls[Layer][1]; ++CurrentCall)
									{
										//printf("\33[2KSetting Block %d, Layer %d, (%d/%d)\r", Block, Layer, CurrentCall - LayerCalls[Layer][0], LayerCalls[Layer][1] - LayerCalls[Layer][0]);

										// First Outputs
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = 0;
										}
										else
										{
											Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = Net->Blocks[Block].FParams.FirstOutputs[CurrentCall - 1][Layer] + OutputSize;
											if((int)Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] > Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
											{
												Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] % (Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2]);
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
															Net->Blocks[Block].FParams.DFEWeights[CurrentCall][pos[CurrentCall]] = Net->Blocks[Block].Weights[Layer][CurrentKernel + Kernel][Channel][y][x];
														}
														else
														{
															Net->Blocks[Block].FParams.DFEWeights[CurrentCall][pos[CurrentCall]] = 0;
														}
														++pos[CurrentCall];
													}
												}
											}
										}
										if((int)Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] + OutputSize > Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
										{
											CurrentKernel++;
										}

										// Mem Control
										Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] = 1 + CurrentCall - LayerCalls[Layer][0];
									}

									break;
						case Pool:

									for(int CurrentCall = LayerCalls[Layer][0]; CurrentCall < LayerCalls[Layer][1]; ++CurrentCall)
									{
										//printf("\33[2KSetting Block %d, Layer %d, (%d/%d)\r", Block, Layer, CurrentCall - LayerCalls[Layer][0], LayerCalls[Layer][1] - LayerCalls[Layer][0]);

										// First Outputs
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = 0;
										}
										else
										{
											Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = Net->Blocks[Block].FParams.FirstOutputs[CurrentCall - 1][Layer] + OutputSize;
										}

										// Mem Control
										Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] = 1 + CurrentCall - LayerCalls[Layer][0];
									}

									break;

						case Fcon:
									for(int CurrentCall = LayerCalls[Layer][0]; CurrentCall < LayerCalls[Layer][1]; ++CurrentCall)
									{
										//printf("\33[2KSetting Block %d, Layer %d, (%d/%d)\r", Block, Layer, CurrentCall - LayerCalls[Layer][0], LayerCalls[Layer][1] - LayerCalls[Layer][0]);

										// First Outputs ( Used as Input Mem Control in this layer)
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = 0;
										}
										else
										{
											Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = Net->Blocks[Block].FParams.FirstOutputs[CurrentCall - 1][Layer] + 1;
											if((int)(BurstSizeDataType * BM * Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer]) >= Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].Dims[Layer][1] * Net->Blocks[Block].Dims[Layer][2])
											{
												Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] = 0;
											}
										}
										
										// Output Mem Control
										if(CurrentCall - LayerCalls[Layer][0] == 0)
										{
											Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] = 1;
										}
										else
										{
											if(Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] == 0)
											{
												Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] = Net->Blocks[Block].FParams.MemControl[CurrentCall - 1][Layer] + 1;
											}
											else
											{
												Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] = Net->Blocks[Block].FParams.MemControl[CurrentCall - 1][Layer];
											}
										}

										// Weights
										for(int i = 0; i < BurstSizeDataType * BM; ++i)
										{
											for(int j = 0; j < OutputSize; ++j)
											{
												if((int)(Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] * BurstSizeDataType * BM + i) < Net->Blocks[Block].Dims[Layer][0] * Net->Blocks[Block].Dims[Layer][1] * Net->Blocks[Block].Dims[Layer][2])
												{
													if((int)(BM * BurstSizeDataType * (Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] - 1) + j) < Net->Blocks[Block].Dims[Layer + 1][0] * Net->Blocks[Block].Dims[Layer + 1][1] * Net->Blocks[Block].Dims[Layer + 1][2])
													{
														Net->Blocks[Block].FParams.DFEWeights[CurrentCall][pos[CurrentCall]] = Net->Blocks[Block].Weights[Layer][0][0][(Net->Blocks[Block].FParams.FirstOutputs[CurrentCall][Layer] * BurstSizeDataType * BM + i)][BM * BurstSizeDataType * (Net->Blocks[Block].FParams.MemControl[CurrentCall][Layer] - 1) + j];
													}
													else
													{
														Net->Blocks[Block].FParams.DFEWeights[CurrentCall][pos[CurrentCall]] = 0;
													}
												}
												else
												{
													Net->Blocks[Block].FParams.DFEWeights[CurrentCall][pos[CurrentCall]] = 0;
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

	// 2.4 --- Add Block --- //

		/*
			Add a Block to the Network

			Net - Network to add a block to

			return value - nothing
		*/

		void AddBlock(Network* Net)
		{
			// --- Save Current Network --- //

				CurrentNet = Net;
				
			// --- Add Block --- //

				++(Net->TotalBlocks);

				Net->Blocks = realloc(Net->Blocks, (Net->TotalBlocks + 1) * (sizeof(Block)));
				if(Net->Blocks == NULL)
				{
					printf("Memory Allocation Error.\n");
					exit(MemoryError);
				}

			// --- Set Dimensions --- //

				// InSize of Block N is OutSize of Block N - 1. Exception is first Block, since it's set in InitCNN
				if(Net->TotalBlocks != 0)
				{
					Net->Blocks[Net->TotalBlocks].Dims = malloc(sizeof(int*));
					if(Net->Blocks[Net->TotalBlocks].Dims == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					Net->Blocks[Net->TotalBlocks].Dims[0] = calloc(3, sizeof(int));
					if(Net->Blocks[Net->TotalBlocks].Dims[0] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					for(int i = 0; i < 3; ++i)
					{
						Net->Blocks[Net->TotalBlocks].Dims[0][i] = Net->Blocks[Net->TotalBlocks - 1].Dims[Net->Blocks[Net->TotalBlocks - 1].BlockSize][i];
					}
				}


			// --- Set Layers --- //

				Net->Blocks[Net->TotalBlocks].Layers = malloc(sizeof(char));

			// --- Set BlockSize --- //

				Net->Blocks[Net->TotalBlocks].BlockSize = 0;

			// --- Set Parallelism --- //

				Net->Blocks[Net->TotalBlocks].FParams.Parallelism = malloc(sizeof(char));

			// --- Set Weights --- //

				Net->Blocks[Net->TotalBlocks].Weights = malloc(sizeof(double****));

			// --- Set LayerParams --- //

				Net->Blocks[Net->TotalBlocks].LayerParams = malloc(sizeof(double*));

			// --- Set DFE Params --- //


				Net->Blocks[Net->TotalBlocks].FParams.NCalls = 0;

				Net->Blocks[Net->TotalBlocks].FParams.FirstOutputs = malloc(sizeof(uint32_t*));
				Net->Blocks[Net->TotalBlocks].FParams.MemControl = malloc(sizeof(uint32_t*));

				Net->Blocks[Net->TotalBlocks].FParams.DFEWeights = malloc(sizeof(double*));

			// --- Set Burst Mult --- //

				if(Net->TotalBlocks != 0)
				{
					SetBurstMult(Net, Net->TotalBlocks - 1, DefBurstMult);
				}
		}

	// 2.5 --- Add Layers --- //

		// 2.5.1 --- Conv --- //

			/*
				Add a Conv Layer to the Current Block

				NKernels - Amount of Kernels in this Layer
				KernelSize - Size of Kernels in this Layer
				Stride - Amount of Pixels Kernel Moves at a time
				Padding - Amount of Pixels Added to Input Volume before Computation
				Parallelism - How many Channels are calculated at once.

				return value - nothing
			*/

			void AddConv(int NKernels, char KernelSize, char Stride, char Padding, char Parallelism)
			{
				// --- Check if CNNInit has been called --- //

					if(CurrentNet == NULL)
					{
						printf("CNNInit() must be called atleast once before AddConv().\n");
						exit(PrecedenceError);
					}

				// --- Check if AddBlock has been called atleast once --- //

					if(CurrentNet->TotalBlocks == -1)
					{
						printf("AddBlock() must be called atleast once before AddConv().\n");
						exit(PrecedenceError);
					}

				// --- Check if Conv Layer can be added --- //

					for(int Block = 0; Block < CurrentNet->TotalBlocks + 1; ++Block)
					{
						for(int Layer = 0; Layer < CurrentNet->Blocks[Block].BlockSize; ++Layer)
						{
							if(CurrentNet->Blocks[Block].Layers[Layer] == Fcon)
							{
								printf("Conv layer cannot be added after Fcon layer.\n");
								exit(CNNConstructionError);
							}
						}
					}

				// --- Check if Parameters are Valid --- //

					if(((CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] - KernelSize + (2 * Padding)) % Stride) != 0)
					{
						printf("Layer %d in Block %d has invalid Dimensions.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("InDims = (%dx%d), KernelSize = %d, Padding = %d, Stride = %d.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1], CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1], KernelSize, Padding, Stride);
						printf("InDims - KernelSize + 2*Padding = %d is not divisible by Stride.\n",(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] - KernelSize + (2 * Padding)));
						exit(CNNConstructionError);
					}

					if(NKernels < 1)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("NKernels has to be greater than or equal to 1\n");
						exit(CNNConstructionError);
					}
					if(KernelSize < 2)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("KernelSize has to be greater than or equal to 2\n");
						exit(CNNConstructionError);
					}
					if(Stride < 1)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Stride has to be greater than or equal to 1\n");
						exit(CNNConstructionError);
					}
					if(Padding < 0)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Padding has to be greater than or equal to 0\n");
						exit(CNNConstructionError);
					}
					if(Parallelism < 1)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Parallelism has to be greater than or equal to 1\n");
						exit(CNNConstructionError);
					}

					if((CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] % Parallelism) != 0)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Input has %d Channels, which does not divide into Parallelism ( %d ) with remainder 0.", CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0], Parallelism);
						exit(CNNConstructionError);
					}

					if((CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] % Parallelism) != 0)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Parallelism (%d) cannot be greater than InputChannels/2 (%d).", Parallelism, (int)(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] / (float)2));
						exit(CNNConstructionError);
					}
 
				// --- Set Layer and Parallelism level --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(char));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(char));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Conv;						// Conv ID num
					CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Parallelism;	// How fast computation runs

				// --- Set Params --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(double*));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Init1D(5);
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = 0;					// 0 means no Act Function (changed if add_act is called)
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] = NKernels;			// How many Kernels
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][2] = KernelSize;			// Kernel size. 2 means 2x2, 3 means 3x3
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][3] = Stride;				// How many pixels Kernel moves at a time
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][4] = Padding;			// How many 0 pixels are added to input before computing

				// --- Init Weights --- //

					int WeightDims[3];
					WeightDims[0] = CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0];				// NChannels
					WeightDims[1] = KernelSize;																												// Kernel Size
					WeightDims[2] = KernelSize;																												// Kernel Size

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(double****));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = malloc( (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * (sizeof(double***) * NKernels) );
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					for(int i = 0; i < NKernels; ++i)		
					{
						CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][i] = Init3D(WeightDims);
						if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][i] == NULL)
						{
							printf("Memory Allocation Error.\n");
							exit(MemoryError);
						}

						RandomizeArray3D(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][i], WeightDims, -0.05, 0.05);
					}
				
				// --- Count number of Layers in this block --- //

					++(CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize);

				// --- Set Dimensions --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(int*));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = calloc(3, sizeof(int));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = NKernels;
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] = 1 + ((CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1] - KernelSize + (2 * Padding))/Stride);
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][2] = CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1];
			}

		// 2.5.2 --- Pool --- //

			/*
				Add a Pool Layer to the Current Block

				FilterSize - Size of Pooling Window
				Type - Type of Pooling
				Stride - Amount of Pixels Kernel Moves at a time

				return value - nothing
			*/

			void AddPool(char FilterSize, char Type, char Stride)
			{
				// --- Check if CNNInit has been called --- //

					if(CurrentNet == NULL)
					{
						printf("CNNInit() has to be called before AddPool().\n");
						exit(PrecedenceError);
					}

				// --- Check if AddBlock has been called atleast once --- //

					if(CurrentNet->TotalBlocks == -1)
					{
						printf("AddBlock() must be called atleast once before AddPool().\n");
						exit(PrecedenceError);
					}

				// --- Check if Pool Layer can be added --- //

					for(int Block = 0; Block < CurrentNet->TotalBlocks + 1; ++Block)
					{
						for(int Layer = 0; Layer < CurrentNet->Blocks[Block].BlockSize; ++Layer)
						{
							if(CurrentNet->Blocks[Block].Layers[Layer] == Fcon)
							{
								printf("Pool layer cannot be added after Fcon layer.\n");
								exit(CNNConstructionError);
							}
						}
					}

				// --- Check if Parameters are Valid --- //

					if(FilterSize < 2)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("FilterSize has to be greater than or equal to 2\n");
						exit(CNNConstructionError);
					}
					if(Type < MaxPool || Type > MeanPool)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Pooling Type Invalid.\n");
						exit(CNNConstructionError);
					}
					if(Stride < 1)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Stride has to be greater than or equal to 1\n");
						exit(CNNConstructionError);
					}

					if(((CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] - FilterSize) % Stride) != 0)
					{
						printf("Layer %d in Block %d has invalid Dimensions.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("InDims = (%dx%d), FilterSize = %d, Stride = %d.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1], CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1], FilterSize, Stride);
						printf("InDims - PoolSize = %d is not divisible by Stride.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1] - FilterSize);
						exit(CNNConstructionError);
					}

				// --- Set Layer and Parallelism --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(char));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(char));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Pool;
					CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = 1;

				// --- Set Params --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(double*));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Init1D(4);
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = 0;					// 0 means no Act Function (changed if add_act is called)
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] = FilterSize;			// Pool Size : 2 means 2x2, 3 means 3x3, etc
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][2] = Type;				// Type: 1 - Max Pooling, 2 - Mean Pooling
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][3] = Stride;				// How many pixels Pooling Block moves at a time
			
				// --- Init Mask --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(double****));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = malloc(sizeof(double***));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(CNNConstructionError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = Init3D(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize]);
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(CNNConstructionError);
					}

				// --- Count number of Layers in this block --- //

					++(CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize);

				// --- Set Dimensions --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(int*));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = calloc(3, sizeof(int));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][0];
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] = 1 + ((CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1] - FilterSize)/(double)Stride);
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][2] = CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1];
			}

		// 2.5.3 --- Fcon --- //

			/*
				Add an Fcon Layer to the Current Block

				Output Size - Amount of Output Neurons
				Parallelism - How fast computation is done

				return value - nothing
			*/

			void AddFcon(int OutputSize, char Parallelism)
			{
				// --- Check if CNNInit has been called --- //

					if(CurrentNet == NULL)
					{
						printf("CNNInit() has to be called before AddFcon().\n");
						exit(PrecedenceError);
					}

				// --- Check if AddBlock has been called atleast once --- //

					if(CurrentNet->TotalBlocks == -1)
					{
						printf("AddBlock() must be called atleast once before AddFcon().\n");
						exit(PrecedenceError);
					}

				// --- Check if Fcon Layer can be added --- //

					if(CurrentNet->TotalBlocks > 0)
					{
						if(CurrentNet->Blocks[CurrentNet->TotalBlocks - 1].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks - 1].BlockSize - 1][0] == Soft)
						{
							printf("No Layers can be added after Fcon layer with Softmax activation\n");
							exit(CNNConstructionError);
						}
					}

				// --- Check if Params are valid --- //

					if(OutputSize < 1)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("OutputSize has to be greater than or equal to 1\n");
						exit(CNNConstructionError);
					}
					if(Parallelism < 1)
					{
						printf("Layer %d in Block %d has invalid Params.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1, CurrentNet->TotalBlocks + 1);
						printf("Parallelism has to be greater than or equal to 1\n");
						exit(CNNConstructionError);
					}

				// --- Set Layer and Parallelism --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(char));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(char));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Fcon;
					CurrentNet->Blocks[CurrentNet->TotalBlocks].FParams.Parallelism[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Parallelism;


				
				//  --- Set Params --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(double*));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = Init1D(2);

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = 0;					// 0 means no Act Function (changed if add_act is called)
					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] = 0;					// Drop Probability

				// --- Init Weights --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(double****));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					int InputSize = CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] * CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] * CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][2];

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = malloc(sizeof(double***));
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = malloc(sizeof(double**));
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0][0] = malloc(sizeof(double*) * InputSize);

					for(int i = 0; i < InputSize; ++i)		
					{
						CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0][0][i] = Init1D(OutputSize);
						RandomizeArray1D(CurrentNet->Blocks[CurrentNet->TotalBlocks].Weights[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0][0][i], OutputSize, -0.05, 0.05);		// Assign Random Values to Weights
					}

				// --- Count number of Layers in this block --- //

					++(CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize);

				// --- Set Dimensions --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims = realloc(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims, (CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize + 1) * sizeof(int*));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] = calloc(3, sizeof(int));
					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] == NULL)
					{
						printf("Memory Allocation Error.\n");
						exit(MemoryError);
					}

					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][0] = 1;
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][1] = 1;
					CurrentNet->Blocks[CurrentNet->TotalBlocks].Dims[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize][2] = OutputSize;
			}

		// 2.5.4 --- Acti --- //

			/*
				Add an Activation Layer to the Current Block
				
				Parallelism - How many Outputs ( In Multiples of 48 ) are Calculated each time the DFE is Ran. Para = 1 means 48 Outputs. If NOutputs < 48 then just make this Param 1

				return value - nothing
			*/

			void AddActi(char Func)
			{
				// --- Check if CNNInit has been called --- //

					if(CurrentNet == NULL)
					{
						printf("CNNInit() has to be called before AddActi().\n");
						exit(PrecedenceError);
					}

				// --- Check if AddBlock has been called atleast once --- //

					if(CurrentNet->TotalBlocks == -1)
					{
						printf("AddBlock() must be called atleast once before AddActi().\n");
						exit(PrecedenceError);
					}

				// --- Check if Activation is valid --- //

					if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][0] != 0)
					{
						printf("Activation Layer relative to layer %d in Block %d is invalid.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] + 1, CurrentNet->TotalBlocks + 1);
						printf("Activation Layer is set twice.\n");
						exit(CNNConstructionError);
					}

					if((CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1] == Conv || CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1] == Pool) && Func == Soft)
					{
						printf("Activation Layer relative to layer %d in Block %d is invalid.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] + 1, CurrentNet->TotalBlocks + 1);
						printf("Conv Layer cannot have Softmax Activation\n");
						exit(CNNConstructionError);
					}

				// --- Set Act function of previous Layer --- //

					CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][0] = Func;

				// Since last layer always has Softmax activation, increment TotalBlocks so it's easier to cycle in future

					if(Func == Soft)
					{
						// --- Set Burst Mult of Last Block --- //
							
							SetBurstMult(CurrentNet, CurrentNet->TotalBlocks, DefBurstMult);
						
						CurrentNet->TotalBlocks++;
					}
			}

		// 2.5.5 --- Drop --- //

			/*
				Add a Dropout to the Current Block
				
				Parallelism - How many Outputs ( In Multiples of 48 ) are Calculated each time the DFE is Ran. Para = 1 means 48 Outputs. If NOutputs < 48 then just make this Param 1

				return value - nothing
			*/

			void AddDrop(double DropP)
			{
				// --- Check if CNNInit has been called --- //

					if(CurrentNet == NULL)
					{
						printf("CNNInit() has to be called before AddDrop().\n");
						exit(PrecedenceError);
					}

				// --- Check if AddBlock has been called atleast once --- //

					if(CurrentNet->TotalBlocks == -1)
					{
						printf("AddBlock() must be called atleast once before AddDrop().\n");
						exit(PrecedenceError);
					}
				
				// Check if Dropout is Valid
				if(CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1] != Fcon)
				{
					printf("Dropout Layer relative to layer %d in Block %d is invalid.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] + 1, CurrentNet->TotalBlocks + 1);
					printf("Dropout Layers can only be present after Fcon Layers.\n");
					exit(CNNConstructionError);
				}

				if(CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1] != 0)
				{
					printf("Dropout Layer relative to layer %d in Block %d is invalid.\n", CurrentNet->Blocks[CurrentNet->TotalBlocks].Layers[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize] + 1, CurrentNet->TotalBlocks + 1);
					printf("Dropout Layer is set twice.\n");
					exit(CNNConstructionError);
				}

				// Set DropP of Previous Layer
				CurrentNet->Blocks[CurrentNet->TotalBlocks].LayerParams[CurrentNet->Blocks[CurrentNet->TotalBlocks].BlockSize - 1][1] = DropP;
			}

// 3 --- Pre Defined Models --- //

	// 3.1 --- AlexNet --- //

		/*
			Sets Network to AlexNet configuration

			Net - Network that gets configured

			return value - nothing
		*/

		void CreateAlexNet(Network* Net)
		{
			int InDims[3] = {3, 227, 227};
			InitCNN(Net, InDims);

			AddBlock(Net);
			AddConv(96, 11, 4, 0, 1);
			AddActi(ReLu);
			AddPool(3, MaxPool, 2);
			AddConv(256, 5, 1, 2, 1);
			AddActi(ReLu);
			AddPool(3, MaxPool, 2);
			
			AddBlock(Net);
			AddConv(384, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(384, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(256, 3, 1, 1, 1);
			AddPool(3, MaxPool, 2);

			AddBlock(Net);
			AddFcon(4096, 1);
			AddActi(ReLu);
			AddDrop(0.5);
			AddFcon(4096, 1);
			AddActi(ReLu);
			AddDrop(0.5);
			AddFcon(1000, 1);
			AddDrop(0.5);
			AddActi(Soft);
		}

	// 3.2 --- VGG16 --- //

		/*
			Sets Network to VGG16 configuration

			Net - Network that gets configured

			return value - nothing
		*/

		void CreateVGG16(Network* Net)
		{
			int InDims[3] = {3, 224, 224};
			InitCNN(Net, InDims);

			AddBlock(Net);
			AddConv(64, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(64, 3, 1, 1, 1);
			AddActi(ReLu);
			AddPool(2, MaxPool, 2);
			
			AddBlock(Net);
			AddConv(128, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(128, 3, 1, 1, 1);
			AddActi(ReLu);
			AddPool(2, MaxPool, 2);

			AddBlock(Net);
			AddConv(256, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(256, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(256, 3, 1, 1, 1);
			AddActi(ReLu);
			AddPool(2, MaxPool, 2);

			AddBlock(Net);
			AddConv(512, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(512, 3, 1, 1, 1);
			AddActi(ReLu);
			AddConv(512, 3, 1, 1, 1);
			AddActi(ReLu);
			AddPool(2, MaxPool, 2);

			AddBlock(Net);
			AddFcon(4096, 1);
			AddActi(ReLu);
			AddFcon(4096, 1);
			AddActi(ReLu);
			AddFcon(1000, 1);
			AddActi(Soft);

			// Possible Burst Mult Setup
			SetBurstMult(Net, 0, 2048);
			SetBurstMult(Net, 1, 1024);
			SetBurstMult(Net, 2, 256);
			SetBurstMult(Net, 3, 64);
			SetBurstMult(Net, 4, 8);
		}

// 4 --- Print Arch --- //

	/*
		Prints the Network Architecture

		return value - nothing
	*/

	void PrintArchitecture(Network* Net)
	{
		char flag = 0;
		printf("\n\nNetwork Architecture:\n\n");
		printf("Total Blocks = %d, InputDims = %dx%dx%d\n\n", Net->TotalBlocks, Net->Blocks[0].Dims[0][0], Net->Blocks[0].Dims[0][1], Net->Blocks[0].Dims[0][1]);
		for(int i = 0; i < Net->TotalBlocks; ++i)
		{
			printf("Block %d : Layer\t  Parameters\t\t  Paralellism\t  OutputSize\n",i + 1);
			for(int j = 0; j < Net->Blocks[i].BlockSize; ++j)
			{
				printf("\t  ");

				switch(Net->Blocks[i].Layers[j])
				{
					case Conv:
							printf("Conv\t  %.0f, %.0fx%.0f, %.0f, %.0f, ", Net->Blocks[i].LayerParams[j][1], Net->Blocks[i].LayerParams[j][2], Net->Blocks[i].LayerParams[j][2], Net->Blocks[i].LayerParams[j][3], Net->Blocks[i].LayerParams[j][4]);
							
							if(Net->Blocks[i].LayerParams[j][0] == 1)
							{
								printf("Relu");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == 2)
							{
								printf("Sig");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == 3)
							{
								printf("Tanh");
							}
							else
							{
								printf("None");
							}
							break;

					case Pool:
							printf("Pool\t  %.0fx%.0f, %.0f, ", Net->Blocks[i].LayerParams[j][1], Net->Blocks[i].LayerParams[j][1], Net->Blocks[i].LayerParams[j][3]);
							
							if(Net->Blocks[i].LayerParams[j][2] == 1)
							{
								printf("Max, ");
							}
							else
							{
								printf("Mean, ");
							}
							
							if(Net->Blocks[i].LayerParams[j][0] == 1)
							{
								printf("Relu");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == 2)
							{
								printf("Sig");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == 3)
							{
								printf("Tanh");
							}
							else
							{
								printf("None");
							}
							break;

					case Fcon:
							printf("Fcon\t  ");
							if(Net->Blocks[i].LayerParams[j][0] == ReLu)
							{
								printf("Relu, ");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == Sigmoid)
							{
								printf("Sig, ");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == Tanh)
							{
								printf("Tanh, ");
							}
							else if(Net->Blocks[i].LayerParams[j][0] == Soft)
							{
								printf("Soft, ");
							}
							else
							{
								printf("None, ");
							}
							printf("%.2f\t", Net->Blocks[i].LayerParams[j][1]);

							flag = 1;
							break;
				}
				if(flag == 0)
				{
					printf("\t  %d\t\t  %dx%dx%d\n", Net->Blocks[i].FParams.Parallelism[j], Net->Blocks[i].Dims[j+1][0], Net->Blocks[i].Dims[j+1][1], Net->Blocks[i].Dims[j+1][1]);
				}
				else
				{
					printf("\t  %d\t\t  1x%d\n", Net->Blocks[i].FParams.Parallelism[j], Net->Blocks[i].Dims[j+1][2]);
				}
			}
			printf("\n");
		}
		printf("\n\nNetwork Architecture Finished!\n");
	}

/*
// 5 --- Save --- //

		void SaveModel(Block* Blocks, int CurrentNet->TotalBlocks)
		{
			for(int Block = 0; Block < CurrentNet->TotalBlocks; Block++)
			{
				for(int Layer = 0; Layer < Blocks[Block].BlockSize; Layer++)
			}
		}

// 6 --- Load --- //

		void LoadModel()
		{

		}
*/
