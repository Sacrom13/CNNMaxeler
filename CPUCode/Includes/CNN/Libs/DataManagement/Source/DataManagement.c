#include "../DataManagement.h"

/*
            File Structure

	1 - Math Operations
		1.1 - Generate Random

    2 - Array Operations

    	2.1 - Init
    		2.1.1 - 1D
    		2.1.2 - 3D

    	2.2 - Randomize
    		2.2.1 - 1D
    		2.2.2 - 3D

    	2.3 - Copy
    		2.3.1 - 1D
    		2.3.2 - 3D

    	2.4 - Convert
    		2.4.1 - 1D
    		2.4.2 - 3D

    	2.5 - Comparisons
    		2.5.1 - 1D
    		2.5.2 - 3D

		2.6 - Array Math
			2.6.1 - Pad
			2.6.1 - Flip

		2.7 - Free
			2.7.1 - 1D
			2.7.2 - 3D
*/

// 1 --- Math Operations --- //

    // 1.1 --- Generate Random Functions --- //

  		/*
			Random Number Generator

            Min - Minimum Value
            Max - Max Value
            
            Return Value - Random number between min and max
        */

		double GenerateRand(double Min, double Max)
		{
			return ( (Max - Min) * ((double)rand() / RAND_MAX) ) + Min;
		}

// 2 --- Array Operations

	// 2.1 --- Init --- //

		// 2.1.1 --- 1D --- //

			/*
				Init 1D Array to given size

	            Dim - Size
	            
	            Return Value - Array with N = Size doubles, all initialized to 0
	        */

			double* Init1D(int Dim)
			{
				return (double*)calloc(Dim, sizeof(double));
			}

		// 2.1.2 --- 3D --- //

			/*
				Init 3D Array to given size

	            Dims - Array Dimensions
	            
	            Return Value - 3D array of doubles, all initialized to 0
	        */

			double*** Init3D(int* Dims)
			{
				double*** Input = (double***) calloc( (sizeof(double**) * Dims[0]) + (sizeof(double*) * Dims[0] * Dims[1]) + (sizeof(double) * Dims[0] * Dims[1] * Dims[2]) , 1);

				for(int i = 0; i < Dims[0]; ++i)
				{
					Input[i] = (double**)(Input + Dims[0]) + i * Dims[1];
					for(int j = 0; j < Dims[1]; ++j)
					{
						Input[i][j] = (double*)(Input + Dims[0] + Dims[0]*Dims[1]) + i*Dims[1]*Dims[2] + j*Dims[2];
					}
				}

				return Input;
			}

	// 2.2 --- Randomize --- //

		// 2.2.1 --- 1D --- //

			/*
				Randomize 1D Array, with random numbers between min and max.

	            Input - Input Array
	            Dim - Input dims
	            Min - minimum value
	            Max - max value
	            
	            Return Value - Nothing
	        */

			void RandomizeArray1D(double* Input, int Dim, double Min, double Max)
			{
				for(int i = 0; i < Dim; ++i)
				{
					Input[i] = GenerateRand(Min, Max);
				}
			}

		// 2.2.2 --- 3D --- //

			/*
				Randomize 3D Array, with random numbers between min and max.

	            Input - Input Array
	            Dims - Input dims
	            Min - minimum value
	            Max - max value
	            
	            Return Value - Nothing
	        */

			void RandomizeArray3D(double*** Input, int* Dims, double Min, double Max)
			{
				for(int i = 0; i < Dims[0]; ++i)
				{
					for(int j = 0; j < Dims[1]; ++j)
					{
						for(int z = 0; z < Dims[2]; ++z)
						{
							Input[i][j][z] = GenerateRand(Min, Max);
						}
					}
				}
			}

	// 2.3 --- Copy --- //

		// 2.3.1 --- 1D --- //

			/*
				Copy 1D Array. Input is source array, Output is new Array

	            Input - Source Array
	            Output - Output Array
	            Dims - Input Dimensions
	            
	            Return Value - Nothing.
	        */

			void Copy1D(double* Input, double* Output, int Dim)
			{
				for(int i = 0; i < Dim; ++i)
				{
					Output[i] = Input[i];
				}
			}

		// 2.3.2 --- 3D --- //

			/*
				Copy 3D Array. Input is source array, Output is new Array

	            Input - Source Array
	            Output - Output Array
	            Dims - Input Dimensions
	            
	            Return Value - Nothing.
	        */

			void Copy3D(double*** Input, double*** Output, int* Dims)
			{
				for(int i = 0; i < Dims[0]; ++i)
				{
					for(int j = 0; j < Dims[1]; ++j)
					{
						for(int k = 0; k < Dims[2]; ++k)
						{
							Output[i][j][k] = Input[i][j][k];
						}
					}
				}
			}

	// 2.4 --- Convert

		// 2.4.1 --- 1D --- //

			/*
				Convert 3D array to 1D

	            Input - Source Array
	            Output - Output Array
	            Dim - input Dimensions
	            
	            Return Value - Nothing.
	        */

			void ConvertTo1D(double*** Input, double* Output, int* Dims)
			{
				int counter = 0;

				for(int i = 0; i < Dims[0]; ++i)
				{
					for(int j = 0; j < Dims[1]; ++j)
					{
						for(int z = 0; z < Dims[2]; ++z)
						{
							Output[counter] = Input[i][j][z];
							counter++;
						}
					}
				}
			}

		// 2.4.2 --- 3D --- //

			/*
				Convert 1D array to 3D

	            Input - Source Array
	            Out - Output Array
	            Dims - Output Dimensions
	            
	            Return Value - Nothing.
	        */

			void ConvertTo3D(double* Input, double*** Output, int* Dims)
			{
				int counter = 0;

				for(int i = 0; i < Dims[0]; ++i)
				{
					for(int j = 0; j < Dims[1]; ++j)
					{
						for(int z = 0; z < Dims[2]; ++z)
						{
							Output[i][j][z] = Input[counter];
							counter++;
						}
					}
				}
			}

	// 2.5 --- Compare --- //

		// 2.5.1 --- 1D --- //

			/*
				Compare two 1D Arrays

	            Input1 - First Array
	            Input2 - Second Array
	            Dim - Dimensions
	            
	            Return Value - Nothing.
	        */

			void Compare1D(double* Input1, double* Input2, int Dim, double Margin)
			{
				for(int i = 0; i < Dim; ++i)
				{
					if(Input1[i] > (Input2[i] + Margin) || Input1[i] < (Input2[i] - Margin) )
					{
						printf("Compare Error. in1[%d] = %f\tin2[%d] = %f\n", i, Input1[i], i, Input2[i]);
						return;
					}
				}
				printf("Arrays Contain the same values!\n");
			}

		// 2.5.2 --- 3D --- //

			/*
				Compare two 3D Arrays

	            Input1 - First Array
	            Input2 - Second Array
	            Dim - Dimensions
	            
	            Return Value - Nothing.
	        */

			void Compare3D(double*** Input1, double*** Input2, int* Dims, double Margin)
			{
				for(int i = 0; i < Dims[0]; ++i)
				{
					for(int j = 0; j < Dims[1]; ++j)
					{
						for(int k = 0; k < Dims[2]; ++k)
						{
							if(Input1[i][j][k] > (Input2[i][j][k] + Margin) || Input1[i][j][k] < (Input2[i][j][k] - Margin) )
							{
								printf("Compare Error. in1[%d][%d][%d] = %.10f\tin2[%d][%d][%d] = %.10f\n", i, j, k, Input1[i][j][k], i, j, k, Input2[i][j][k]);
								return;
							}
						}
					}
				}
				printf("Arrays Contain The same values!\n");
			}

	// 2.6 --- Array Math --- //

		// 2.6.1 --- Pad --- //

			/*
				Pad Array with Padding Dims

	            Input1 - Input Array
	            Output - Padded Array
	            Dims - Input Array Dimensions
	            Padding - Amount of pixels to Pad
	            
	            Return Value - Nothing.
	        */

			void Pad(double*** Input, double*** Output, int* Dims, char Padding)
			{   
			    for (int i = 0; i < Dims[0]; ++i)
			    {
			        for (int j = 0; j < Dims[1]; ++j)                  
			        {
			            for (int k = 0; k < Dims[1]; ++k) 
			            {
			                Output[i][j + Padding][k + Padding] = Input[i][j][k];
			            }
			        }
			    }
			}

		// 2.6.2 --- Flip --- //

			/*
				Flip Array Diagonally

	            Input - Input Array
	            Output - Flipped Array
	            Dim - Input Array Dimensions
	            
	            Return Value - Nothing.
	        */

			void Flip(double*** Input, double*** Output, int* Dims)
			{
			    double aux;
			    
			    for(int Channels = 0; Channels < Dims[0]; ++Channels)
			    {
			        aux = 0;

			        // Reverse Columns
			        for(int Row = Dims[1] - 1; Row >= 0; --Row)
			        {
			            for(int Col = 0; Col < Dims[2]; ++Col)
			            {
			                Output[Channels][Row][Col] = Input[Channels][(int)aux][Col];
			            }
			            aux++;
			        }

			        // Reverse Rows
			        for(int Row = 0; Row < Dims[1]; ++Row)
			        {
			            for(int Col = 0; Col < Dims[2] / 2; ++Col)
			            {
			                aux = Output[Channels][Row][Col];
			                Output[Channels][Row][Col] = Output[Channels][Row][Dims[1] - Col - 1];
			                Output[Channels][Row][Dims[1] - Col - 1] = aux;
			            }
			        }
			    }
			}

	// 2.7 --- Free --- //

		// 2.7.1 --- 1D --- //

		/*
			Frees 1D Array

            Input - Input Array
            
            Return Value - Nothing.
        */

		void Free1D(double* Input)
		{
			free(Input);
		}

		// 2.7.2 --- 3D --- //

		/*
			Frees 3D Array

            Input - Input Array
            
            Return Value - Nothing.
        */

        void Free3D(double*** Input)
        {
        	free(Input);
		}
