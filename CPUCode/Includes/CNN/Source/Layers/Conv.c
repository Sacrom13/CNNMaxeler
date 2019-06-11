#include "../../CNN.h"

/*
                File Structure
                
    1 - Math Operations
        1.1 - Convolution

    2 - Layer Propagation
        2.1 - Forward Propagation
        2.2 - Backward Propagation
*/


// 1 --- Math operations Needed for this Layer --- //

    // 1.1 --- Convolution --- //
    
        /*
            Calculate Convolution between Input and Filters

            Input - Input Volume
            Filters - Kernel Weights
            StartX - X position of Input
            StartY - Y position of Input
            FilterSize - KernelSize ( 2 means 2x2, 3 means 3x3, etc...)

            Return Value - Convolution Result
        */

        static double Convolution(double** Input, double** Filters, int StartX, int StartY, int FilterSize)
        {
        	double out = 0;

            // Convolve Kernel with Pixels
            for(int y = 0; y < FilterSize; ++y)
            {
                for(int x = 0 ; x < FilterSize; ++x)
                {
                    out += Input[StartY + y][StartX + x] * Filters[y][x];
                }
            }

        	return out;
        }

// 2 --- Layer Propagation --- //

    // 2.1 --- Forward Propagation --- //

        /*
            Calculate Conv Layer Forward Propagation 

            Input - Input Volume
            InDims - Input Volume Dimensions
            Output - Where to place Output
            Filters - Kernel Weights
            Params - LayerParams
            [0] = Act;                // 0 means no Act Function (changed if add_act is called)
            [1] = NKernels;           // How many Kernels
            [2] = KernelSize;         // Kernel size. 2 means 2x2, 3 means 3x3
            [3] = Stride;             // How many pixels Kernel moves at a time
            [4] = Padding;            // How many 0 pixels are added to input before computing


            Return Value - Nothing
        */

        void ConvForwCpu(double*** Input, int* InDims,                   // Input
                         double*** Output,                               // Output
                         double**** Filters, double* Params)             // Weights + Params

        {
            // --- Pad Input --- //

                // Input Dimensions
                int PadDims[3];
                PadDims[0] = InDims[0];
                PadDims[1] = InDims[1] + 2 * Params[4];
                PadDims[2] = PadDims[1];

                // Padding Operation
                double*** Padded = Init3D(PadDims);
                Pad(Input, Padded, InDims, Params[4]);

            // --- Convolution --- //

                for(int kernel = 0; kernel < Params[1]; ++kernel)
                {
                    // Output Counters
                    int outy = 0;
                    int outx = 0;

                    for(int y = 0; y <= PadDims[1] - Params[2]; y += Params[3])             // Input img Y
                    {
                        for(int x = 0; x <= PadDims[1] - Params[2]; x += Params[3])         // Input img X
                        {
                            for(int channel = 0; channel < InDims[0]; ++channel)            // Input img channels
                            {
                                // --- Calc Convolution --- //

                                    Output[kernel][outy][outx] += Convolution(Padded[channel], Filters[kernel][channel], x, y, Params[2]);
                            }

                            // --- Apply Act Func --- //

                                if(Params[0] == ReLu)
                                {
                                    Output[kernel][outy][outx] = Output[kernel][outy][outx] > 0 ? Output[kernel][outy][outx] : 0;
                                }
                                else if(Params[0] == Sigmoid)
                                {
                                    Output[kernel][outy][outx] = 1/(double)(1 + exp(-Output[kernel][outy][outx]));
                                }
                                else if(Params[0] == Tanh)
                                {
                                    Output[kernel][outy][outx] = tanh(Output[kernel][outy][outx]);
                                }

                            // Go Right
                            ++outx;
                        }
                        
                        // Come Back
                        outx = 0;

                        // Move Down
                        ++outy;
                    }
                }

            // --- Free --- //

                Free3D(Padded);
        }

    // 2.2 --- Backward Propagation --- //

        /*
            Calculate Conv Layer Backward Propagation

            PrevInput - Input Volume from previous ForwardPropagation
            InDims - PrevInput Dimensions
            PrevOutput - Output Volume from previous ForwardPropagation
            OutDims - Output  Dimensions
            Error - Error from Next Layer
            Output - Output ( Error to backprop onto previous Layer)
            Filters - Weights
            Params - LayerParams
                [0] = Act;                // 0 means no Act Function (changed if add_act is called)
                [1] = NKernels;           // How many Kernels
                [2] = KernelSize;         // Kernel size. 2 means 2x2, 3 means 3x3
                [3] = Stride;             // How many pixels Kernel moves at a time
                [4] = Padding;            // How many 0 pixels are added to input before computing
            LearningRate - LearningRate

            Return Value - Nothing
        */

        void ConvBackCpu(double*** PrevInput, int* InDims,                           // Varibles to Calculate Weight Updates
                         double*** PrevOutput, int* OutDims, double*** Error,         // Variables to Calculate Delta
                         double*** Output,                                           // Variable to Store Error from this layer
                         double**** Filters, double* Params,                          // Weights + Params
                         double LearningRate)                                        // Learning Rate
        {
            /* Params
            [0] = Act;                 // 0 means no Act Function (changed if add_act is called)
            [1] = NKernels;           // How many Kernels
            [2] = KernelSize;         // Kernel size. 2 means 2x2, 3 means 3x3
            [3] = Stride;             // How many pixels Kernel moves at a time
            [4] = Padding;            // How many 0 pixels are added to input before computing
            */
            
            // --- Apply Act Func and Setup Delta --- //
    
                // Activation Function is applied with a "Shortcut".
                // Sigmoid For example. Forward means Sigmoid(x)
                // Sigmoid Derivative = Sigmoid(x) * (1 - Sigmoid(x))
                // The "Shortcut" means we simply take x * (1 - x), since the x we get as Input Parameter 
                // is the one after activation function, and not before.


                //  If Stride > 1 instead of a Full Convolution in step 2.1, 
                //  a Fractionally Strided Convolution needs to be done instead.
                //  A Fractionally Strided Convolution is the same as a Full Convolution,
                //  aslong as Delta is setup Correctly.
                //
                //  If Stride is 2 for example, we need to pad Delta like so.
                //
                //  d1  0   d2  0   d3
                //  0   0   0   0   0
                //  d4  0   d5  0   d6
                //  0   0   0   0   0
                //  d7  0   d8  0   d9
                //
                //  One possible way to calculate Error dimensions is by the Equation
                //  Size + (Size - 1) * (Stride - 1)
                //  3 + ( 3 - 1 ) * (2 - 1) = 5

                // Calculate DeltaDimensions
                int DeltaDims[3];
                DeltaDims[0] = OutDims[0];
                DeltaDims[1] = OutDims[1] + ((OutDims[1] - 1) * (Params[3] - 1));
                DeltaDims[2] = DeltaDims[1];

                // Init array with zeroes
                double*** Delta = Init3D(DeltaDims);

                // Put the Value ( Depending on Activation Function ) in the Correct Spots
                if(Params[0] == ReLu)
                {
                    for(int channel = 0; channel < OutDims[0]; ++channel)
                    {  
                        for(int y = 0; y < OutDims[1]; ++y)
                        {
                            for(int x = 0; x < OutDims[1]; ++x)
                            {
                                Delta[channel][(int) (y * Params[3])][(int) (x * Params[3])] =  PrevOutput[channel][x][y] > 0 ? Error[channel][y][x] : 0;
                            }
                        }
                    }
                }
                else if(Params[0] == Sigmoid)
                {
                    for(int channel = 0; channel < OutDims[0]; ++channel)
                    {
                        for(int y = 0; y < OutDims[1]; ++y)
                        {
                            for(int x = 0; x < OutDims[1]; ++x)
                            {
                                Delta[channel][(int) (y * Params[3])][(int) (x * Params[3])] = PrevOutput[channel][y][x] * (1 - PrevOutput[channel][y][x]) * Error[channel][y][x];
                            }
                        }
                    }
                }
                else if(Params[0] == Tanh)  
                {   
                    for(int channel = 0; channel < OutDims[0]; ++channel)
                    {
                        for(int y = 0; y < OutDims[1]; ++y)
                        {
                            for(int x = 0; x < OutDims[1]; ++x)
                            {
                                Delta[channel][(int) (y * Params[3])][(int) (x * Params[3])] = (1 - pow(PrevOutput[channel][y][x], 2)) * Error[channel][y][x];
                            }
                        }
                    }
                }
                else            // No activation Function
                {
                    for(int channel = 0; channel < OutDims[0]; ++channel)
                    {
                        for(int y = 0; y < OutDims[1]; ++y)
                        {
                            for(int x = 0; x < OutDims[1]; ++x)
                            {
                                Delta[channel][(int) (y * Params[3])][(int) (x * Params[3])] =  Error[channel][y][x];
                            }
                        }
                    }
                }

        	// 2 --- Calculate Output and Update Weights--- //

                // Full Convolution between Delta and Weights.
                // To Calculate this, we first need to Pad Delta by KernelSize - 1, so the Full Convolution can be done like a regular convolution

                // Calculate Dimensions
                int DeltaPadDims[3];
                DeltaPadDims[0] = DeltaDims[0];
                DeltaPadDims[1] = DeltaDims[1] + 2 * (Params[2] - 1);
                DeltaPadDims[2] = DeltaPadDims[1];

                // Apply Padding
                double*** DeltaPadded = Init3D(DeltaPadDims);
                Pad(Delta, DeltaPadded, DeltaDims, Params[2] - 1);

                // Now that Delta is padded, we can Calculate the Full Convolution by doing a Normal Convolution with DeltaPadded instead of Delta

                // Output has InDims[0] Channels. Delta has NKernels Channels. Weights have Nkernels size and InDims[0] Channels.
                // In forward propagation, In[InChannel] connects to Out[OutChannel] via Filters[OutChannel][InChannel]. 
                // To do this Backwards, to Calculate Out[InChannel] need to sum Delta[OutChannel]*Filter[OutChannel][InChannel] for all OutChannels(NKernels).

                // Each iteration Calculates 1 Output Channel
            	for (int channel = 0; channel < InDims[0]; ++channel)          
                {   
                    // Counters to place Output values in correct spots
                    int outy = 0;                               
                    int outx = 0;

                    // Go through Y and X. Since after this operation we would need to remove padding(if > 1),
                    // we can start the FullConv at (Y,X) = (Padding, Padding) and end at (Y, X) = (OutDims - KernelSize - Pad, OutDims - KernelSize - Pad)
                    // so the padding points which would be removed aren't calculated.
                    for(int y = Params[4]; y <= DeltaPadDims[1] - Params[2] - Params[4]; ++y)
                    {
                        for(int x = Params[4]; x <= DeltaPadDims[1] - Params[2] - Params[4]; ++x)
                        {
                            // We go Through all the Kernels for this Output Channel, and sum the output of respective convolution results
                            for(int kernel = 0; kernel < Params[1]; ++kernel)       
                            {
                                // Convolution
                                Output[channel][outy][outx] += Convolution(DeltaPadded[kernel], Filters[kernel][channel], x, y, Params[2]);
                            }

                            // Go right
                            ++outx;
                        }

                        // Come Back
                        outx = 0;

                        // Go Down
                        ++outy;
                    }
                }

            // --- Update Weights --- //

                // Convolution between PrevInput with Delta ( Padded according to Stride, as mentioned in the Apply Act Section ) as the Filter

                // Before computing, we need to Pad PrevInput, so Dimensions are correct

                int InPadDims[3];
                InPadDims[0] = InDims[0];
                InPadDims[1] = InDims[1] + 2*Params[4];
                InPadDims[2] = InPadDims[1];

                double*** PrevInputPadded = Init3D(InPadDims);
                Pad(PrevInput, PrevInputPadded, InDims, Params[4]);

                // Delta[i] corresponds to Kernel i
                // Channel j of Kernel i is linked to Delta[i] via Input[j]
                // For each Output Channel, go through all the Channels and update Filters[i][j] using convolution between Input[j] and Delta[i]

                // Each iteration Calculates the Update to 1 Kernel
                for(int kernel = 0; kernel < Params[1]; ++kernel)
                {
                    // Update Each Channel of the Kernel with the Output
                    for(int channel = 0; channel < InDims[0]; ++channel)
                    {
                        // Go Through X and Y as a Normal Convolution
                        for(int y = 0; y <= InPadDims[1] - DeltaPadDims[1]; ++y)
                        {
                            for(int x = 0; x < InPadDims[1] - DeltaPadDims[1]; ++x)
                            {
                                // Convolution
                                Filters[kernel][channel][y][x] -= (LearningRate * Convolution(PrevInputPadded[channel], DeltaPadded[kernel], x, y, DeltaPadDims[1])); 
                            }
                        }
                    }
                }

            // --- Weight Updates Calculated! --- //

            // --- Free --- //

                Free3D(Delta);
                Free3D(DeltaPadded);
                Free3D(PrevInputPadded);
        }
