#ifndef DFENETWORK_DEFINED
#define DFENETWORK_DEFINED

	// 1 --- Required Libs --- //

		#include <unistd.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include "../../../Libs/CNNLibs.h"

	// 2 --- Extra Libs --- //

		#include "DFEBlocks/DFEBlocks.h"
		#include <MaxSLiCInterface.h>	// Simple Live CPU interface

	// 3 --- Default Parameters --- //

		// 3.1 --- Frequencies --- //

			#define DefDesignFreq 100
			#define DefLMemFreq 100
		// 3.2 --- Burst Size --- //

			#define BurstSizeBytes 192
			#define BurstSizeDataType 24

	// 4 --- Structures --- //
	
		// 4.1 --- DFEPropagation Params --- //

			// 4.1.1 --- Forward Prop Params --- //

				typedef struct
				{
					int InputOffset;			// Input Offset in LMem

					int* Parallelism;			// Level of Parallelism of each layer
		  			int* BurstMult;				// Multiplier for DFEBurstSize. BurstSize for Galava is 192 Bytes. if DFEBurstMult = 2 then 192*2 Bytes are Calculated at once

					uint32_t NCalls;			// How many times DFE has to be ran for this Block to finish Computation

					uint32_t** FirstOutputs;	// First Output Point for a given Layer of a given Call
					uint32_t** MemControl;		// Call Counter for DFE

					double** DFEWeights;		// Weight Setup for each DFE Iteration

				} DFEForwParams;

		// 4.2 --- Backward Prop Params --- //

				typedef struct
				{
					int InputOffset;			// Input Offset in LMem

					int* Parallelism;			// Level of Parallelism of each layer
					int* BurstMult;				// Multiplier for DFEBurstSize. BurstSize for Galava is 192 Bytes. if DFEBurstMult = 2 then 192*2 Bytes are Calculated at once

				} DFEBackParams;
#endif
