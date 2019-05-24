#include "../../CNN.h"

/*
                File Structure
                
    1 - Math Operations
        1.1 - PoolWindow

    2 - Layer Propagation
        2.1 - Forward Propagation
        2.2 - Backward Propagation
*/

// 1 --- Math Operations --- //

    // 1.1 --- Pool Window --- //

        /*
            Calculate Pool value of a Window Size

            Input - Input Volume
            StartX - X position of Input
            StartY - Y position of Input
            Type - Max/Mean Pool
            WindowSize - WindowSize ( 2 means 2x2, 3 means 3x3, etc...)
            Mask - Mask for Backward Propagation

            Return Value - Pool Result
        */

        static double PoolWindow(double** Input, int StartX, int StartY, char Type, char WindowSize, double** Mask)
        {
            double out;
            switch(Type)
            {
                case MaxPool:     // Max pooling

                        // Start out with minimum possible value for a double
                        out = -DBL_MAX;

                        int MaxX = 0, MaxY = 0;

                        // Go Through Window and find highest Value
                        for(int y = StartY; y < StartY + WindowSize; ++y)
                        {
                            for(int x = StartX; x < StartX + WindowSize; ++x)
                            {
                                if(Input[y][x] > out)
                                {
                                    out = Input[y][x];

                                    // Store Position of Maximum Value
                                    MaxY = y;
                                    MaxX = x;
                                }
                                // Reset Mask
                                Mask[y][x] = 0;
                            }
                        }
                        // Set 1 in Position where the Max Value was found
                        Mask[MaxY][MaxX] = 1;
                        
                        break;

                case MeanPool:     // Mean pooling

                        // Start out with 0
                        out = 0;

                        // Sum all Values in Window
                        for(int y = StartY; y < StartY + WindowSize; ++y)
                        {
                            for(int x = StartX; x < StartX + WindowSize; ++x)
                            {
                                out += Input[y][x];

                                // Reset Mask
                                Mask[y][x] = 0;
                            }
                        }

                        // Divide by WindowSize to Calculate Average
                        out /= (WindowSize * WindowSize);

                        // Place Average Value of Window in all Values of Mask
                        for(int y = StartY; y < StartY + WindowSize; ++y)
                        {
                            for(int x = StartX; x < StartX + WindowSize; ++x)
                            {
                                Mask[y][x] = out;
                            }
                        }

                        break;
            }
            
            return out;
        }

// 2 --- Layer Propagation --- //

    // 2.1 --- Forward Propagation --- //

        /*
            Calculate Pooling Layer Backprop

            Input - Input Volume
            InDims - Input Dimensions
            Mask - Mask for Backprop
            Output - Output Volume
            Params - LayerParams
                [0] = ActFunc               // 0 - None, 1 - Relu, 2 - Sigmoid, 3 - Tanh
                [1] = WindowSize            // Pool Size : 2 means 2x2, 3 means 3x3, etc
                [2] = Type;                 // Type: 1 - Max Pooling, 2 - Mean Pooling
                [3] = Stride;               // How many pixels Pooling Block moves at a time
                
            Return Value - Pool Result
        */


        void PoolForwCpu(double*** Input, int* InDims,           // Input
                         double*** Mask,                         // Mask to fill up
                         double*** Output,                       // Output
                         double* Params)                         // Params
        {
            

            // Processing cicle
            for(int channel = 0; channel < InDims[0]; ++channel)
            {
                // Output Counters
                int outx = 0;
                int outy = 0;
                
                // Go Through entire Input
                for(int y = 0; y <= InDims[1] - Params[1]; y += Params[3])
                {
                    for(int x = 0; x <= InDims[1] - Params[1]; x += Params[3])
                    {

                        // --- Pool --- //

                            Output[channel][outy][outx] = PoolWindow(Input[channel], x, y, Params[2], Params[1], Mask[channel]);

                        // --- Act Func --- //

                            if(Params[0] == ReLu)
                            {
                                Output[channel][outy][outx] = Output[channel][outy][outx] > 0 ? Output[channel][outy][outx] : 0;
                            }
                            else if(Params[0] == Sigmoid)
                            {
                                Output[channel][outy][outx] = 1/(double)(1 + exp(-Output[channel][outy][outx]));
                            }
                            else if(Params[0] == Tanh)
                            {
                                Output[channel][outy][outx] = tanh(Output[channel][outy][outx]);
                            }

                        // Go Right
                        ++outx;
                    }

                    // Come Back
                    outx = 0;

                    // Go Down
                    ++outy;
                }
            }
        }

    // 2.2 --- Backward Propagation --- //

        /*
            Calculate Pool Layer Backprop

            Input - Input Volume
            InDims - Input Dimensions
            Mask - Mask for Backprop
            Output - Output Volume
            Params - LayerParams
                [0] = ActFunc               // 0 - None, 1 - Relu, 2 - Sigmoid, 3 - Tanh
                [1] = WindowSize            // Pool Size : 2 means 2x2, 3 means 3x3, etc
                [2] = Type;                 // Type: 1 - Max Pooling, 2 - Mean Pooling
                [3] = Stride;               // How many pixels Pooling Block moves at a time
                
            Return Value - Pool Result
        */

        void PoolBackCpu(double*** PrevOutput, int* OutDims, double*** Error,                         // Variable to Calculate Delta
                         double*** Mask,                                                             // Variable to Calculate this layer Error
                         double*** Output,                                                           // Variable to Store this layer Error
                         double* Params)                                                             // Params
        {

            // --- Apply Act Func and Prepare Delta --- //

                double*** Delta = Init3D(OutDims);

                    // Put the Value ( Depending on Activation Function ) in the Correct Spots 
                    if(Params[0] == ReLu)
                    {
                        for(int channel = 0; channel < OutDims[0]; ++channel)
                        {  
                            for(int y = 0; y < OutDims[1]; ++y)
                            {
                                for(int x = 0; x < OutDims[1]; ++x)
                                {
                                    Delta[channel][y][x] =  PrevOutput[channel][x][y] > 0 ? Error[channel][y][x] : 0;
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
                                    Delta[channel][y][x] = (PrevOutput[channel][y][x] * 1 - PrevOutput[channel][y][x]) * Error[channel][y][x];
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
                                    Delta[channel][y][x] = (1 - pow(PrevOutput[channel][y][x], 2)) * Error[channel][y][x];
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
                                    Delta[channel][y][x] =  Error[channel][y][x];
                                }
                            }
                        }
                    }

            // --- Calculate Output --- //

                // Go Through Entire Output. Select Window from Mask. 
                // Multiply the Value of the Output relative to that Window by the Values of Mask
                // Mask Contains 1 for the spot where the value was selected and 0 otherwise (Max Pool), in all spots of the Window
                // or the Average ( Mean Pool), in all spots of the Window

                // Go Through Output
                for (int channel = 0; channel < OutDims[0]; ++channel) 
                {
                    for (int y = 0; y < OutDims[1]; ++y) 
                    {
                        for (int x = 0; x < OutDims[2]; ++x) 
                        {
                            // Select Window
                            for(int MaskY = 0; MaskY < Params[1]; ++MaskY)
                            {
                                for(int MaskX = 0; MaskX < Params[1]; ++MaskX)
                                {
                                    int outy = (int)(y * Params[1]) + MaskY;
                                    int outx = (int)(x * Params[1]) + MaskX;

                                    Output[channel][outy][outx] = Mask[channel][outy][outx] * Delta[channel][y][x];
                                }
                            }
                        }
                    }
                }

            // --- Free --- //

                Free3D(Delta);
        }
