#include "../../CNN.h"

/*
                File Structure
                
    1 - Global Variables
    	1.1 - Design Parameters
    	1.2 - Network Parameters

    2 - Parameter Definition
    	2.1 - Design Parameters
			2.1.1 - Design Freq
			2.1.2 - LMem Freq
			2.1.3 - Pipelining

    3 - Write Parameters
		3.1 - Write Layers
		3.2 - Write Params

	4 - Propagation
		4.1 - Not Training
			4.1.1 - Block Forward
			4.1.2 - CNN Forward
		4.2 - Training
			4.2.1 - Forward
				5.2.1.1 - Block Forward
				5.2.1.1 - CNN Forward
			4.2.2 - Backward
				4.2.2.1 - Block Backward
				4.2.2.2 - CNN Backward

	5 - Network Performance
		5.1 - Classify
		5.2 - Accuracy
			5.2.1 - Accuracy
			5.2.2 - TestAccuracy
	
	6 - Training
		6.1 - Train

*/

// 1 --- Global Variables --- //

	// 1.1 --- Design Parameters --- //

		static int DesignFreq = DefFreq;					// Frequency the design will run at
		static int LMemFreq = DefFreq;						// LMem Access Frequency
		static int Pipelining = DefPip;						// Pipelining Level

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

		// 2.1.3 --- Pipelining --- //

			/*
				Set Pipelining Level

				Pip - Pipelining

				return value - nothing
			*/

			void SetPipelining(int Pip)
			{
				Pipelining = Pip;
			}

// 3 --- Write Parameters --- //

	// 3.1 --- Write Layers --- //

		/*
			Write Layers of a Block to a File

			Block - Block to Write

			return value - nothing
		*/

		static void WriteLayers(Block Block, char* Filename)
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
					fprintf(fp, "%d %d ", Block.Layers[i], Block.FParams.Parallelism[i]);

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

			// --- Write Block Output Dimensions + BurstMultiplier --- //

				fprintf(fp, "0 %d %d %d\n", Block.Dims[Block.BlockSize][0], Block.Dims[Block.BlockSize][2], Block.FParams.BurstMult);

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

				fprintf(fp, "%d %d %d\n", LMemFreq, DesignFreq, Pipelining);

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

// 4 --- Propagation --- //

	// 4.1 ---  Not Training --- //

		// 4.1.2 --- Block Forward --- //

			/*
				Calculate Output of a Block

				Block - Block
				Inputs - Input to the Block
				Flag1D - Flag to Signal Dimension Switches

				return value - Output to be used as Input to next Block
			*/

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

		// 4.1.2 --- CNN Forward --- //

			/*
				Calculate Network Output

				Net - Network to be used
				Input - Input to the Network

				return value - Network Output
			*/

			static double* CNNForwardCpu(Network Net, double*** Input)
			{
				// --- Setup for Computation --- //

					char* Flag1D = malloc(sizeof(char));
					*Flag1D = 0;

					double**** BlockOutputs = malloc((Net.TotalBlocks + 1) * sizeof(double***));

					BlockOutputs[0] = Init3D(Net.Blocks[0].Dims[0]);
					Copy3D(Input, BlockOutputs[0], Net.Blocks[0].Dims[0]);

				// --- Go Through all Blocks and place value in Output --- //

					for(int Block = 0; Block < Net.TotalBlocks; ++Block)
					{
						BlockOutputs[Block + 1] = BlockForwardCpu(Net.Blocks[Block], BlockOutputs[Block], Flag1D);

						Free3D(BlockOutputs[Block]);
					}

					double* Output = Init1D(Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2]);
					Copy1D(BlockOutputs[Net.TotalBlocks][0][0], Output, Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2]);

				// --- Free --- //

					Free3D(BlockOutputs[Net.TotalBlocks]);
					free(BlockOutputs);

					free(Flag1D);

				return Output;
			}

	// 4.2 --- Training --- //

		// 4.2.1 --- Forward --- //

			// 4.2.1.1 --- Block Forward --- //

				/*
					Same as 5.1.1 (Block Forward), but Stores Output of every layer, to be used later in training

					Block - Block to Forward
					Input - Input to the Block
					Flag1D - Flag to signal Dimensional Switches
					LayerOutputs - Variable to store Layer Outputs

					return value - nothing
				*/

				static void BlockForwardCpuTrain(Block Block, double*** Input, char* Flag1D, double**** LayerOutputs)
				{
					LayerOutputs[0] = Init3D(Block.Dims[0]);
					Copy3D(Input, LayerOutputs[0], Block.Dims[0]);

					// --- Go Through Every Layer in the Block --- //

						for(int Layer = 0; Layer < Block.BlockSize; ++Layer)
						{
							LayerOutputs[Layer + 1] = Init3D(Block.Dims[Layer + 1]);
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
																1);
												Free1D(Aux);

												*Flag1D = 1;
											}
											FconForwCpu(LayerOutputs[Layer][0][0], Block.Dims[Layer][0] * Block.Dims[Layer][1] * Block.Dims[Layer][2], 
															LayerOutputs[Layer + 1][0][0], Block.Dims[Layer + 1][2], 
															Block.Weights[Layer][0][0], Block.LayerParams[Layer], 
															1);
											break;
							}
						}
				}

			// 4.2.1.2 --- CNN Forward --- //

				/*
					Same as 5.1.2 (CNN Forward), but Stores Output of every layer, to be used later in training

					Net - Network to be used
					Input - Input to the Network
					BlockLayerOutputs - Variable to store Layer Outputs

					return value - nothing
				*/

				static void CNNForwardCpuTrain(Network Net, double*** Input, double***** BlockLayerOutputs)
				{
					char* Flag1D = malloc(sizeof(char));
					*Flag1D = 0;

					BlockLayerOutputs[0] = malloc(sizeof(double***) * (Net.Blocks[0].BlockSize + 1));

					// --- Go Through Every Block and Save Layer Outputs --- //

						// First Block Outside so we don't have to constantly check for it in the Cycle
						BlockForwardCpuTrain(Net.Blocks[0], Input, Flag1D, BlockLayerOutputs[0]);

						for(int Block = 1; Block < Net.TotalBlocks; ++Block)
						{
							BlockLayerOutputs[Block] = malloc(sizeof(double***) * (Net.Blocks[Block].BlockSize + 1));
							BlockForwardCpuTrain(Net.Blocks[Block], BlockLayerOutputs[Block - 1][Net.Blocks[Block - 1].BlockSize], Flag1D, BlockLayerOutputs[Block]);
						}

					// --- Free --- //

						free(Flag1D);
				}

		// 4.2.2 --- Backward --- //

			// 4.2.2.1 --- Block Backward --- //

				/*
					Calculate Backpropagation of a Block

					Block - Block to Forward
					BlockError - Input to the Block
					LayerOutputs - LayerOutputs from Previous Forward Propagation

					return value - Error to Backpropagate onto Next Block
				*/

				static double*** BlockBackwardCpu(Block Block, double*** BlockError, double**** LayerOutputs, double LearningRate)
				{
					double**** Error = malloc(sizeof(double***) * (Block.BlockSize + 1));
					Error[Block.BlockSize] = Init3D(Block.Dims[Block.BlockSize]);

					Copy3D(BlockError, Error[Block.BlockSize], Block.Dims[Block.BlockSize]);

					// 1 --- Go Through Every Layer in the Block, in Backwards Order --- //

						for(int Layer = Block.BlockSize - 1; Layer >= 0; --Layer)
						{
							Error[Layer] = Init3D(Block.Dims[Layer]);

							switch(Block.Layers[Layer])
							{
								case Conv:
											ConvBackCpu(LayerOutputs[Layer], Block.Dims[Layer],
				                 						LayerOutputs[Layer + 1], Block.Dims[Layer + 1], Error[Layer + 1],
				                						Error[Layer],
				                						Block.Weights[Layer], Block.LayerParams[Layer], 
				                						LearningRate);
				                 			break;

								case Pool:
											PoolBackCpu(LayerOutputs[Layer + 1], Block.Dims[Layer + 1], Error[Layer + 1],
										                 Block.Weights[Layer][0],
										                 Error[Layer],
										                 Block.LayerParams[Layer]);
											break;

								case Fcon:
											
											// Switch Dims if Necessary
											if(Block.Dims[Layer][0] != 1)
											{
												int InDim = Block.Dims[Layer][0] * Block.Dims[Layer][1] * Block.Dims[Layer][2];
												int InDims[3] = {1, 1, InDim};

												double* OutputErrorAux = Init1D(InDims[2]);

												double* PrevInputAux = Init1D(InDims[2]);
												ConvertTo1D(LayerOutputs[Layer], PrevInputAux, Block.Dims[Layer]);

												FconBackCpu(PrevInputAux, InDims[2],
															LayerOutputs[Layer + 1][0][0], Block.Dims[Layer + 1][2], Error[Layer + 1][0][0],
															OutputErrorAux,
															Block.Weights[Layer][0][0], Block.LayerParams[Layer], 
															LearningRate);

												ConvertTo3D(OutputErrorAux, Error[Layer], Block.Dims[Layer]);

												Free1D(OutputErrorAux);
												Free1D(PrevInputAux);
											}
											else
											{
												FconBackCpu(LayerOutputs[Layer][0][0], Block.Dims[Layer][2],
															LayerOutputs[Layer + 1][0][0], Block.Dims[Layer + 1][2], Error[Layer + 1][0][0],
															Error[Layer][0][0],
															Block.Weights[Layer][0][0], Block.LayerParams[Layer], 
															LearningRate);
											}
											break;
							}

							// Do some Freeing Here so we don't have to cycle again
							Free3D(Error[Layer + 1]);
						}

					double*** Output = Init3D(Block.Dims[0]);

					Copy3D(Error[0], Output, Block.Dims[0]);

					// --- Free --- //

						Free3D(Error[0]);
						free(Error);

					return Output;
				}

			// 4.2.2.2 --- CNN Backward --- //

				/*
					Backpropagate Network on given Input
					
					Input - Input to Backpropagate on
					Label - Label for Input

					return value - Nothing
				*/

				static void CNNBackwardCpu(Network Net, double*** Input, double* Label)
				{
					// --- Run Forward Propagation and get All Needed Data --- //

						// Store Layer Outputs
						double***** BlockLayerOutputs = malloc(sizeof(double****) * Net.TotalBlocks);

						// Forward Prop
						CNNForwardCpuTrain(Net, Input, BlockLayerOutputs);
						
						// Get Error
						double* Error;
						Error = ErrorBackward(BlockLayerOutputs[Net.TotalBlocks - 1][Net.Blocks[Net.TotalBlocks - 1].BlockSize][0][0], Label, Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2], Net.EFunc);

						// Store Layer Errors
						double**** BlockErrors = malloc((Net.TotalBlocks + 1) * sizeof(double***));

						// Init Layer Errors so we don't have to check for it in the cycle
						BlockErrors[Net.TotalBlocks] = Init3D(Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize]);
						Copy1D(Error, BlockErrors[Net.TotalBlocks][0][0], Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2]);

					// --- Go Through All Blocks --- //

						for(int Block = Net.TotalBlocks - 1; Block >= 0; --Block)
						{
							BlockErrors[Block] = BlockBackwardCpu(Net.Blocks[Block], BlockErrors[Block + 1], BlockLayerOutputs[Block], Net.LearningRate);

							// Do some Freeing Here so we don't have to cycle again
							Free3D(BlockErrors[Block + 1]);

							for(int Layer = 0; Layer < Net.Blocks[Block].BlockSize + 1; ++Layer)
							{
								Free3D(BlockLayerOutputs[Block][Layer]);
							}
							free(BlockLayerOutputs[Block]);
						}

					// --- Free --- //

						Free1D(Error);
						free(BlockLayerOutputs);

						Free3D(BlockErrors[0]);
						free(BlockErrors);
				}

// 5 --- Network Performance --- //

	// 5.1 --- Classify --- //

		/*
			Get CNN Classification of a given Input

			Input - Input to Classify
			Network - Network to be used for Classification

			return value - Class Input belongs to.
		*/

		int Classify(Network Net, double*** Input)
		{
			// --- Forward Input Through Network --- //

				double* Prediction = CNNForwardCpu(Net, Input);

			// --- Find Highest Value and save Index --- //

				double MaxPrediction = Prediction[0];
				int MaxPredictionIndex = 0;

				for(int i = 1; i < Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2]; ++i)
				{
					if(Prediction[i] > MaxPrediction)
					{
						MaxPrediction = Prediction[i];
						MaxPredictionIndex = i;
					}
				}

			Free1D(Prediction);

			return MaxPredictionIndex;
		}

	// 5.2 --- Accuracy --- //

		// 5.2.1 --- Accuracy --- //

			/*
				Calculate Wether a Prediction is Correct

				Prediction - Prediction from Network
				Truth - Label for the Input

				return value - 1 if Correct, 0 if Wrong
			*/

			static double CalcAccuracy(Network Net, double* Prediction, double* Truth)
			{
				double MaxPrediction = Prediction[0];
				double MaxTruth = Truth[0];

				double MaxPredictionJ = 0;
				double MaxTruthJ = 0;

				// --- Find Highest Value of Prediction and Label --- //

					for(int j = 1; j < Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2]; ++j)
					{
						// --- Find Max of Prediction --- //

							if(Prediction[j] > MaxPrediction)
							{
								MaxPrediction = Prediction[j];
								MaxPredictionJ = j;
							}

						// --- Find Max of Truth --- //

							if(Truth[j] > MaxTruth)
							{
								MaxTruth = Truth[j];
								MaxTruthJ = j;
							}
					}

				// --- Check if They Match --- //
				if(MaxPredictionJ == MaxTruthJ)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

		// 5.2.2 --- TestAccuracy --- //

			/*
				Calculate Accuracy of a given Dataset

				Net - Network to be used
				Inputs - Inputs to Test
				Truth - Labels of each Input
				NSamples - How many Inputs to Test

				return value - Network Accuracy on the given DataSet
			*/

			double CalcTestAccuracy(Network Net, double**** Inputs, double** Labels, int NSamples)
			{
				double Accuracy = 0;

				double TotalTime = 0;

				double* Prediction;

				printf("Current Accuracy -> %d/%d (%.2f%%)\n", (int)Accuracy, 0, 100*Accuracy);
				printf("Elapsed Time = %.2fs\n", TotalTime/1000000);


				// --- Go Trough every Sample in the DataSet--- //

					for(int i = 0; i < NSamples; ++i)
					{
						StartTiming();

						// --- Get Network Prediction Prediction --- //

							Prediction = CNNForwardCpu(Net, Inputs[i]);

							double MaxPrediction = Prediction[0];
							double MaxPredictionJ = 0;

							double MaxLabel = Labels[i][0];
							double MaxLabelJ = 0;

						for(int j = 1; j < Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2]; ++j)
						{
							// --- Find Max of Prediction --- //

								if(Prediction[j] > MaxPrediction)
								{
									MaxPrediction = Prediction[j];
									MaxPredictionJ = j;
								}

							// --- Find Max of Truth --- //

								if(Labels[i][j] > MaxLabel)
								{
									MaxLabel = Labels[i][j];
									MaxLabelJ = j;
								}
						}

						// --- Check if Correct --- //

							if(MaxPredictionJ == MaxLabelJ)
							{
								Accuracy++;
							}

						// --- Free --- //

							Free1D(Prediction);

						TotalTime += StopTiming();

						printf("\033[F\33[2K\033[F\33[2K\033[F\33[2K");
						printf("Sample -> %d/%d\n", i + 1, NSamples);
						printf("Current Accuracy -> %d/%d (%.2f%%)\n", (int)Accuracy, i, 100*Accuracy/(i + 1));
						printf("Elapsed Time = %.2fs\n", TotalTime/1000000);
					}

				return 100*Accuracy/NSamples;
			}

// 6 --- Train --- //

	/*
		Train Network
		
		Net - Network to be used
		Inputs - Training DataSet. Dimensions need to be {DataSize, InDims}
		Labels - Labels. Dimensions need to be {DataSize, NClasses}
		DataSize - How many Inputs the Training DataSet contains
		MaxEpochs - Maximum Amount of Epochs to run Training for
		GoalError - Target Error
		GoalAccuracy - Target Accuracy

		return value - Nothing
	*/

	void CNNTrainCPU(Network Net, double**** Inputs, double** Labels, int DataSize, int MaxEpochs, double GoalError, double GoalAccuracy)
	{
		double TotalTime = 0;
		double Epochs = 0;
		double Error = 0;
		double Accuracy = 0;

		double BestError = FLT_MAX;
		double BestAccuracy = 0;

		printf("Elapsed Time: %.4f s\n", TotalTime/1000000);
		printf("Epoch %.2f:\n", Epochs);
		printf("\tCurrent\t\tBest\n");
		printf("Err\t%.2f\t\t%.2f\n", Error, BestError);
		printf("Acc\t%.2f%%\t\t%.2f%%\n", Accuracy, BestAccuracy);

		double* Prediction;

		// --- Start Training --- //

			while(1)
			{
				StartTiming();

					// --- Take BatchSize random samples from DataSet --- //

						for(int i = 0; i < Net.BatchSize; ++i)
						{
							int Sample = round(GenerateRand(0, DataSize - 1));

							// --- Forward BatchSize random samples from DataSet --- //

								Prediction = CNNForwardCpu(Net, Inputs[Sample]);

							// --- Keep Statistics --- //

								Error += ErrorForward(Prediction, Labels[Sample], Net.Blocks[Net.TotalBlocks - 1].Dims[Net.Blocks[Net.TotalBlocks - 1].BlockSize][2], Net.EFunc);
								Accuracy += CalcAccuracy(Net, Prediction, Labels[Sample]);

							// --- Backprop --- //

								CNNBackwardCpu(Net, Inputs[Sample], Labels[Sample]);

							// --- Free --- //
								
								Free1D(Prediction);
						}

				// 1.2 --- Update Statistics --- //

					Epochs += (Net.BatchSize/(double)DataSize);
					Error /= (double) Net.BatchSize;
					Accuracy /= (double) (0.01 * Net.BatchSize);

					if(Error < BestError)
					{
						BestError = Error;
					}
					if(Accuracy >= BestAccuracy)
					{
						BestAccuracy = Accuracy;
					}

				TotalTime += StopTiming();

				// 1.3 --- Print Statistics to User --- //

				printf("\033[F\33[2K\033[F\33[2K\033[F\33[2K\033[F\33[2K\033[F\33[2K");

				if(Epochs > MaxEpochs)
				{
					printf("Max Epochs Reached!\n");
					return;
				}
				if(BestError <= GoalError)
				{
					printf("Error reached Goal!\n");
					return;
				}
				if(BestAccuracy >= GoalAccuracy)
				{
					printf("Accuracy reached Goal!\n");
					return;
				}

				printf("Elapsed Time: %.4f s\n", TotalTime/1000000);
				printf("Epoch %.2f:\n", Epochs);
				printf("\tCurrent\t\tBest\n");
				printf("Err\t%.2f\t\t%.2f\n", Error, BestError);
				printf("Acc\t%.2f%%\t\t%.2f%%\n", Accuracy, BestAccuracy);

				Error = 0;
				Accuracy = 0;
			}
	}

	
	static void BlockForwardDFE(Block Block)
	{
		// --------------------------------------------- //
		// ----------     	Basic Static	   ----------//
		// --------------------------------------------- //

			// --------------------------------------------- //
			// ---------- 		Computation    	   ----------//
			// --------------------------------------------- //


			for(int CurrentCall = 0; CurrentCall < (int)Block.FParams.NCalls; ++CurrentCall)
			{
				printf("Running Call %d/%d.\n", CurrentCall + 1, Block.FParams.NCalls);

				// Running Blocking version.
				MovingAverageSimple_RunForwardBlock(
						Block.FParams.InputOffset,
						Block.FParams.FirstOutputs[CurrentCall],
						Block.FParams.MemControl[CurrentCall],
						Block.FParams.DFEWeights[CurrentCall]);

				// To run NonBlock version
					//max_nowait(MovingAverageSimple_RunForward_nonblock(
					//	Block.FParams.InputOffset,
					//	Block.FParams.FirstOutputs[CurrentCall],
					//	Block.FParams.MemControl[CurrentCall],
					//	Block.FParams.DFEWeights[CurrentCall]));

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
				Actions.param_FirstOutputs = Block.FParams.FirstOutputs[CurrentCall];
				Actions.param_InputOffset = Block.FParams.InputOffset;
				Actions.param_MemControl = Block.FParams.MemControl[CurrentCall];
				Actions.param_Weights = Block.FParams.DFEWeights[CurrentCall];


				printf("Running Call %d/%d.\n", CurrentCall + 1, Block.FParams.NCalls);
				MovingAverageSimple_RunForward_run(Engine, &Actions);
			}
			printf("\n");*/
	}

	double* CNNForwardDFE(Network Net, double*** Input)
	{

		// Write Network Design Parameters
		WriteDesignParams();

		// Setup Input

		int InDims1D = Net.Blocks[0].Dims[0][0] * Net.Blocks[0].Dims[0][1] * Net.Blocks[0].Dims[0][2];

		int OutputSizeDataType = (Net.Blocks[0].FParams.BurstMult * BurstSizeDataType);

		if(InDims1D % OutputSizeDataType != 0)
		{
			InDims1D += (OutputSizeDataType - (InDims1D % OutputSizeDataType));
		}
		double* Input1D = Init1D(InDims1D);
		ConvertTo1D(Input, Input1D, Net.Blocks[0].Dims[0]);

		// Write Input to Memory

			printf("Writing to DFE\n");

			MovingAverageSimple_MemWrite(InDims1D, 0, Input1D);

		// DFE Computation

			printf("Running DFE\n");

			char* Filename = malloc(sizeof(char) * 10);
			Filename = "layers0.txt";

			StartTiming();
			for(int Block = 0; Block < 1; ++Block)
			{
				// Write Block info
				WriteLayers(Net.Blocks[Block], Filename);

				BlockForwardDFE(Net.Blocks[Block]);

				Filename = "layers1.txt";
			}
			printf("DFE Finished. Time Taken = %.2f milliseconds\n", StopTiming()/1000);

		// Read Output from Mem

			printf("Reading from DFE\n");

			int OutputStart = 0;
			for(int i = 0; i < Net.Blocks[0].BlockSize; ++i)
			{
				int DimAux = Net.Blocks[0].Dims[i][0] * Net.Blocks[0].Dims[i][1] * Net.Blocks[0].Dims[i][2];
				if(DimAux % OutputSizeDataType != 0)
				{
					DimAux += (OutputSizeDataType - (DimAux % OutputSizeDataType));
				}

				OutputStart += DimAux;
			}

			int OutDims1D;
			OutDims1D = Net.Blocks[0].Dims[Net.Blocks[0].BlockSize][0] * Net.Blocks[0].Dims[Net.Blocks[0].BlockSize][1] * Net.Blocks[0].Dims[Net.Blocks[0].BlockSize][2];
			if(OutDims1D % OutputSizeDataType != 0)
			{
				OutDims1D += (OutputSizeDataType - (OutDims1D % OutputSizeDataType));
			}

			double* Output = Init1D(OutDims1D);
			MovingAverageSimple_MemRead(OutDims1D,
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
