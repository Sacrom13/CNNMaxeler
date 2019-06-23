/*
 * Network.h
 *
 *  Created on: Jun 19, 2019
 *      Author: specs
 */

#ifndef NETWORK_DEFINED
#define NETWORK_DEFINED

	// 1 --- Network Libs --- //

		#include "CPU/CPUNetwork.h"
		#include "DFE/DFENetwork.h"

	// 2 --- Structures --- //

		// 2.1 --- Block --- //

		typedef struct
		{
			int** Dims;					// Dimensions of data in this Block

			char* Layers;				// Array Containing Layers
			int BlockSize;				// Size of Layers and Dims

			double***** Weights;		// Weights for Layers that have them. For pooling Layer this will hold the Mask

			double** LayerParams;		// Arrays Containing Layer Parameters

		} Block;

		// 2.2 --- Network --- //

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

	// 4 --- Default Parameters --- //

		#define DefBatchSize 64
		#define DefLearningRate 0.01
		#define DefMomentum 0.01
		#define DefEFunc CrossEnt

	// 5 --- Function Prototypes --- //

		// 5.1 --- CPU --- //

			int Classify(Network Net, double*** Input);
			double CalcTestAccuracy(Network Net, double**** Inputs, double** Labels, int NSamples);

			void CNNTrainCPU(Network Net, double**** Inputs, double** Labels, int DataSize, int MaxEpochs, double GoalError, double GoalAccuracy);

		// 5.2 --- DFE --- //

			void SetLMemFreq(int Freq);
			void SetDesignFreq(int Freq);

			void DFECompile(Network* Net, int** BurstMult, int** ForwParallelism, int** BackParallelism);

			double* CNNForwardDFE(Network Net, double*** input);

#endif
