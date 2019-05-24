#include "../../CNN.h"

/*
                File Structure

    1 - Error Functions ( MSE, Cross Entropy)
        1.1 - Mean Squared Error
        1.2 - Cross Entropy

    2 - API to link with other files
*/

// 1 --- Error Functions --- //

    // 1.1 --- Mean Squared Error --- //

        
        /*
            Calculate (Real - Predicted)^2

            Prediction - Prediction ( YHat )
            Truth - Label ( Y)
            Dim - Size of Prediction and Truth

            Return Value - Error for specific Prediction and Label
        */
        static double MSEForward(double* Prediction, double* Truth, int Dim)
        {
            double Output = 0;

            for(int i = 0; i < Dim; ++i)
            {
                Output += pow(Truth[i] - Prediction[i], 2);
            }

            return Output;
        }


        /* 
            Calculate Derivative of (Real - Predicted)^2 = 2*(Real - Predicted), with respect to Input.
            
            Prediction - Prediction ( YHat )
            Truth - Label ( Y)
            Output - Where result is placed
            Dim - Size of Prediction and Truth

            Return Value - Nothing
        */
        static void MSEBackward(double* Prediction, double* Truth, double* Output, int Dim)
        {
            for(int i = 0; i < Dim; ++i)
            {
                for(int k = 0; k < Dim; ++k)
                {
                    if(i == k)
                    {
                        Output[i] += ( 2 * Prediction[i] * (Truth[i] - Prediction[i]) * (1 - Prediction[i]) );
                    }
                    else
                    {
                        Output[i] -= ( 2 * Prediction[i] * (Truth[k] - Prediction[k])  * Prediction[k]);
                    }
                }
            }
        }

    // 1.2 --- Cross Entropy --- //

        /*
            Calculate ( -Real * ln(prediction))

            Prediction - Prediction ( YHat )
            Truth - Label ( Y)
            Dim - Size of Prediction and Truth

            Return Value - Error for specific Prediction and Label
        */
        static double CrossEntForward(double* Prediction, double* Truth, int Dim)
        {
            double Output = 0;

            for(int i = 0; i < Dim; ++i)
            {
                Output -= Truth[i] * log(Prediction[i]);
            }

            return Output;
        }


        /* 
            Calculate Derivative ( Still not 100% sure on Math.)
            
            Prediction - Prediction ( YHat )
            Truth - Label ( Y)
            Output - Where result is placed
            Dim - Size of Prediction and Truth

            Return Value - Nothing
        */
        static void CrossEntBackward(double* Prediction, double* Truth, double* Output, int Dim)
        {
            for(int i = 0; i < Dim; i++)
            {
                // Doing this if else is the same as. Prediction - Truth. However, this makes network not train so.
                /*
                    if(Truth[i] == 1)
                    {
                        Output[i] = Prediction[i] - 1;
                    }
                    else
                    {
                        Output[i] = Prediction[i];
                    }
                */
                Output[i] = Prediction[i] - Truth[i]; 
            }
        }

// 2 --- API with CNN.c --- //

    /* 
        // Calculate Error for a given prediction and label,
        according to a certain Error Function

        Prediction - Prediction ( YHat )
        Truth - Label ( Y)
        Output - Where result is placed
        Dim - Size of Prediction and Truth

        Return Value - Error for the Prediction
    */

    double ErrorForward(double* Prediction, double* Truth, int Dim, char EFunc)
    {
        double Output = 0;

        switch(EFunc)
        {
            case MSE:
                        Output = MSEForward(Prediction, Truth, Dim);
                        break;

            case CrossEnt:
                        Output = CrossEntForward(Prediction, Truth, Dim);
                        break;
        }

        return Output;
    }

     /* 
        // Calculate Error Derivative with respect to a given Input, 
        for a given prediction and label, according to a certain Error Function

        Prediction - Prediction ( YHat )
        Truth - Label ( Y)
        Output - Where result is placed
        Dim - Size of Prediction and Truth

        Return Value - Array of size Dim containing Derivative for every Input
    */

    double* ErrorBackward(double* Prediction, double* Truth, int Dim, char EFunc)
    {
        double* Output = Init1D(Dim);
        
        if(EFunc == CrossEnt)
        {
            CrossEntBackward(Prediction, Truth, Output, Dim);
        }
        else if(EFunc == MSE)
        {
            MSEBackward(Prediction, Truth, Output, Dim);
        }

        return Output;
    }

// --- API Done --- //
