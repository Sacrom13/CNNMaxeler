#include "../Debugging.h"

/*
            File Structure 

	1 - Global Variables

	2 - Random Seed

	3 - Toggle Debug Mode
	
    3 - Print 
   		3.1 - 1D
   		3.2 - 3D

   	4 - SetRandomSeed
*/

// 1 --- Global Variables --- //

	char Debug;

// 2 -- Generate Random Seed --- //

	static unsigned long GenerateSeed(unsigned long a, unsigned long b, unsigned long c)
	{
	    a=a-b;  a=a-c;  a=a^(c >> 13);
	    b=b-c;  b=b-a;  b=b^(a << 8);
	    c=c-a;  c=c-b;  c=c^(b >> 13);
	    a=a-b;  a=a-c;  a=a^(c >> 12);
	    b=b-c;  b=b-a;  b=b^(a << 16);
	    c=c-a;  c=c-b;  c=c^(b >> 5);
	    a=a-b;  a=a-c;  a=a^(c >> 3);
	    b=b-c;  b=b-a;  b=b^(a << 10);
	    c=c-a;  c=c-b;  c=c^(b >> 15);
	    return c;
	}

// 3 --- Debug Mode --- //

	/*
		Toggles the Value of Debug

		Return Value - Nothing. 

	*/


	void ToggleDebugMode()
	{
		static char Flag = 0;

		if(!Flag)
		{
			// Init Random Seed
			srand(GenerateSeed(clock(), time(NULL), getpid()));

			// Set Output Buffer to always print as soon as data is written
			setbuf(stdout, NULL);

			// Init Debug
			Debug = 0;

			// Toggle Flag
			Flag = 1;
		}

		// Toggle Debug Mode
		Debug = !Debug;
	}

// 3 --- Print --- //

	// 3.1 --- 1D --- //

		/*
			Prints 1D Array

            Input - Input Array
            Dim - Dimension
            
            Return Value - Nothing.
        */

		void Print1DMatrix(double* Input, int Dim)
		{
			/*for(int i = 0; i < Dim; ++i)
			{
				printf("%d    ", 100 + i);
			}
			printf("\n");
			sleep(1);*/
			for(int i = 0; i < Dim; ++i)
			{
				printf("%.3f  ", Input[i]);
			}
			printf("\n\n");
		}

	// 3.2 --- 3D --- //

		/*
			Prints 3D Array

            Input - Input Array
            Dims - Dimensions
            
            Return Value - Nothing.
        */

		void Print3DMatrix(double*** Input, int* Dims)
		{
			for(int i = 0; i < Dims[0]; ++i)
			{
				printf("Channel %d\n", i + 1);

				for(int j = 0; j < Dims[1]; ++j)
				{
					for(int z = 0; z < Dims[2]; ++z)
					{
						printf("%.3f ", Input[i][j][z]);
					}
					printf("\n");
				}
				printf("\n");
			}
			printf("\n\n");
		}

// 4 --- 
