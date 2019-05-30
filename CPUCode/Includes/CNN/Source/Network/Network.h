#ifndef NETWORK_DEFINED
#define NETWORK_DEFINED

	// 1 --- Required Libs --- //

		#include <unistd.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include <stdint.h>
		#include "../../Libs/CNNLibs.h"

	// 2 --- Structures --- //
	
		// 2.1 --- DFEPropagation Params --- //

			// 2.1.1 --- Forward Prop Params --- //

				typedef struct
				{
					int InputOffset;			// Input Offset in LMem

					char* Parallelism;			// Level of Parallelism of each layer

		  			int BurstMult;				// Multiplier for DFEBurstSize. BurstSize for Galava is 192 Bytes. if DFEBurstMult = 2 then 192*2 Bytes are Calculated at once

					uint32_t NCalls;			// How many times DFE has to be ran for this Block to finish Computation

					uint32_t** FirstOutputs;	// First Output Point for a given Layer of a given Call
					uint32_t** MemControl;		// Call Counter for DFE

					double** DFEWeights;		// Weight Setup for each DFE Iteration

				} DFEForwParams;

		// 2.2 --- Backward Prop Params --- //

				typedef struct
				{
					
				} DFEBackParams;

		// 2.2 --- Block --- //

			typedef struct
			{
				int** Dims;					// Dimensions of data in this Block

				char* Layers;				// Array Containing Layers
				int BlockSize;				// Size of Layers and Dims

				double***** Weights;		// Weights for Layers that have them. For pooling Layer this will hold the Mask

				double** LayerParams;		// Arrays Containing Layer Parameters

				DFEForwParams FParams;		// ForwardParams for DFE
				DFEBackParams BParams;		// BackwardParams for DFE
			} Block;

		// 2.3 --- Network --- //

			typedef struct
			{
				Block* Blocks;				// Network Blocks

				int TotalBlocks;			// Size of Blocks

				int BatchSize;				// How many inputs we consider each time while training
				double LearningRate;		// How fast Network Learns
				double Momentum;			// How much previous changes to Weights influence current iteration
				char EFunc;					// Error function used to calculate error;

			} Network;

	// 3 --- Error Codes --- //

		#define CNNConstructionError -1
		#define DesignError -2
		#define FileError -3
		#define DirectoryError -4
		#define MemoryError -5
		#define PrecedenceError -6
		#define RepeatError -7

	// 4 --- IDs --- //
		
		// 4.1 --- Layers --- //

			#define Conv 1
			#define Pool 2
			#define Fcon 3
		
		// 4.2 --- Layer Params --- //

			// 4.2.1 --- Act Funcs --- //

				#define ReLu 1
				#define Sigmoid 2
				#define Tanh 3
				#define Soft 4

			// 4.2.2 --- Max Pool --- //

				#define MaxPool 1
				#define MeanPool 2

			// 4.2.3 --- Error Funcs --- //

				#define CrossEnt 1
				#define MSE 2

	// 5 --- Default Parameters --- //

		// 5.1 --- Design --- //

			#define DefFreq 100
			#define DefPip 1
			#define DefBurstMult 1

			#define BurstSizeBytes 192				// Burst Size in Bytes, implicit for Galava DFE
			#define BurstSizeDataType 24			// Burst Size in Doubles

		// 5.2 --- Network --- //

			#define DefBatchSize 64
			#define DefLearningRate 0.01
			#define DefMomentum 0.01
			#define DefEFunc CrossEnt

	// 6 --- Function Prototypes --- //

			void SetLMemFreq(int Freq);
			void SetDesignFreq(int Freq);
			void SetPipelining(int Pip);

			int Classify(Network Net, double*** Input);
			double CalcTestAccuracy(Network Net, double**** Inputs, double** Labels, int NSamples);

			void CNNTrainCPU(Network Net, double**** Inputs, double** Labels, int DataSize, int MaxEpochs, double GoalError, double GoalAccuracy);

			double* CNNForwardDFE(Network Net, double*** input);
#endif
