#include "../../CNN.h"

/*
            File Structure

    1 - Layer Propagation
        1.1 - Forward Propagation
        1.2 - Backward Propagation
*/


// 1 --- Layer Propagation --- //

	// 1.1 --- Forward Propagation --- //
	
	    /*
			Calculate Fcon Layer Backpropagation

	        Input - Input Volume
	        InDim - Input Dimensions
	        Output - Output Volume
	        OutDim - Output Dimensions
	        Weights - Fcon Weights
	        Params - LayerParams
			Set Params - 
				[0] = Act Func;					// 0 means no Act Function (changed if add_act is called)
				[1] = DropP;					// Drop Probability
				[2] = Outputs;					// How many Outputs Calculated at once in DFE
			DropControl - Wether to Apply Dropout or not

	        Return Value - nothing
	    */

		void FconForwCpu(double* Input, int InDim, 				// Input
						 double* Output, int OutDim, 			// Output
						 double** Weights, double* Params,		// Weights + Params
						 char DropControl)						// Control Dropout
		{

			// Max in case Softmax
			double Max = -FLT_MAX;


			for(int y = 0; y < OutDim; ++y)
			{
				// If Output is meant to be Dropped out, don't calculate it
				if(GenerateRand(0, 1) < Params[1] && DropControl == 1)
				{
					Output[y] = 0;
					continue;
				}

				// --- Calculate Output --- //

					for(int x = 0; x < InDim; ++x)
					{
						Output[y] += Input[x] * Weights[x][y];
					}

				// --- Apply Act Func --- //

			        if(Params[0] == ReLu)
			        {
			            Output[y] = Output[y] > 0 ? Output[y] : 0;
			        }
			        else if(Params[0] == Sigmoid)
			        {
			            Output[y] = 1/(double)(1 + exp(-Output[y]));
			        }
			        else if(Params[0] == Tanh)
			        {
			            Output[y] = tanh(Output[y]);
			        }
			        if(Params[0] == Soft)
			        {
			        	if(Output[y] > Max)
			        	{
			        		Max = Output[y];
			        	}
			        }
			}

			// Finish Soft Layer Computations
			if(Params[0] == Soft)
			{
				double Sum = 0;

				// Find Sum
				for(int y = 0; y < OutDim; ++y)
				{
					Output[y] = exp(Output[y] - Max);
					Sum += Output[y];
				}
				for(int y = 0; y < OutDim; ++y)
				{
					Output[y] /= Sum;
				}
			}
		}

	// 1.2 --- Backward Propagation --- //
	
	    /*
	        Calculate Fcon Layer Backward Propagation

	        PrevInput - Input Volume from forward propagation
	        InDim - Input Dimensions
	        PrevOutput - Output Volume from forward propagation
	        OutDim - Output Dimensions
	        Error - Error from next Layer
	        Output - Error to Backpropagate onto previous Layer
	        Weights - Fcon Weights
	        Params - LayerParams
			Set Params - 
				[0] = Act Func;					// 0 means no Act Function (changed if add_act is called)
				[1] = DropP;					// Drop Probability
				[2] = Outputs;					// How many Outputs Calculated at once in DFE
			LearningRate - LearningRate

	        Return Value - nothing
	    */

		void FconBackCpu(double* PrevInput, int InDim,							// Variables to Calculate Weight Updates
						 double* PrevOutput, int OutDim, double* Error, 			// Variables to Calculate Delta
						 double* Output,											// Variable to Store this Layer Error
						 double** Weights, double* Params, 						// Weights + Params
						 double LearningRate)									// Learning Rate
		{
			/* Set Params
			[0] = Act Func;					// 0 means no Act Function (changed if add_act is called)
			[1] = DropP;					// Drop Probability
			[2] = Outputs;					// How many Outputs Calculated at once in DFE
			*/

			// --- Apply Act Func and Setup Delta --- //

				double* Delta = Init1D(OutDim);

				if(Params[0] == ReLu)
		        {
		        	for(int i = 0; i < OutDim; ++i)
					{
		            	Delta[i] =  PrevOutput[i] > 0 ? Error[i] : 0;
		           	}
		        }
		        else if(Params[0] == Sigmoid)
		        {
		        	for(int i = 0; i < OutDim; ++i)
					{
			            Delta[i] = PrevOutput[i] * (1 - PrevOutput[i]) * Error[i];
			        }
				}
		        else if(Params[0] == Tanh)  
		        {   
		        	for(int i = 0; i < OutDim; ++i)
					{
			            Delta[i] = (1 - pow(PrevOutput[i], 2)) * Error[i];
		            }
		        }
		        else	// Either Soft or no activation func
		        {   
		        	Copy1D(Error, Delta, OutDim);
				}

			// --- Calculate Output and Update Weights --- //

				for(int i = 0; i < InDim; ++i)
			    {
			        for(int j = 0; j < OutDim; ++j)
			        {
			        	// --- Calculate Output --- //

			            	Output[i] += (Delta[j] * Weights[i][j]);

			            // ---  Update Weights --- //
			            	
			            	Weights[i][j] -= (LearningRate * (Delta[j] * PrevInput[i]));
			        }
			    }

		   	// --- Free --- //

			    Free1D(Delta);
		}
