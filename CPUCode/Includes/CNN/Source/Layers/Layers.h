#ifndef LAYERS_DEFINED
#define LAYERS_DEFINED

	// 1 --- Required Libs --- //

		#include <math.h>
		#include <float.h>
		#include "../../Libs/CNNLibs.h"

	// 2 --- Ids --- //

		// 2.1 --- Layers --- //

			#define Conv 1
			#define Pool 2
			#define Fcon 3

		// 2.2 --- Act Funcs --- //

			#define ReLu 1
			#define Sigmoid 2
			#define Tanh 3
			#define Soft 4

		// 2.3 --- Pool types --- //

			#define MaxPool 1
			#define MeanPool 2

	// 3 --- Overflow Control --- //

			#define MaxValue 1000

	// 4 --- Function Prototypes --- //

		// 4.1 --- Conv --- //

			void ConvForwCpu(double*** Input, int* InDims,           // Input
		                     double*** Output,                       // Output
		                     double**** Filters, double* Params);     // Weights + Params

			void ConvBackCpu(double*** PrevInput, int* InDims,                           // Varibles to Calculate Weight Updates
		                     double*** PrevOutput, int* OutDims, double*** Error,     	// Variables to Calculate Delta
		                     double*** Output,                                       	// Variable to Store Error from this layer
		                     double**** Filters, double* Params,                      	// Weights + Params
		                     double LearningRate);                                    	// Learning Rate

		// 4.2 --- Fcon --- //

			void FconForwCpu(double* Input, int InDim, 				// Input
							 double* Output, int OutDim, 			// Output
							 double** Weights, double* Params,		// Weights + Params
							 char DropControl);						// Control Dropout

			void FconBackCpu(double* PrevInput, int InDim,								// Variables to Calculate Weight Updates
							 double* PrevOutput, int OutDim, double* Error, 				// Variables to Calculate Delta
							 double* Output,												// Variable to Store this Layer Error
							 double** Weights, double* Params, 							// Weights + Params
							 double LearningRate);										// Learning Rate

		// 4.3 --- Pool --- //

			void PoolForwCpu(double*** Input, int* InDims,           // Input
	                         double*** Mask,                         // Mask to fill up
	                         double*** Output,                       // Output
	                         double* Params);                        // Params

			void PoolBackCpu(double*** PrevOutput, int* OutDims, double*** Error,     	// Variable to Calculate Delta
			                 double*** Mask,                                             // Variable to Calculate this layer Error
			                 double*** Output,                                           // Variable to Store this layer Error
			                 double* Params);                                            // Params
			
#endif        
