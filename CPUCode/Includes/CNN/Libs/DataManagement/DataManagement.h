#ifndef DATAMANAGING_DEFINED
#define DATAMANAGING_DEFINED

	// 1 --- Required Libs --- //

		#include "Libs/DataManagementLibs.h"

	// 2 --- Function Prototypes --- //

			double GenerateRand(double Min, double Max);

			double* Init1D(int Dim);
			double*** Init3D(int* Dims);

			void RandomizeArray1D(double* Input, int Dim, double Min, double Max);
			void RandomizeArray3D(double*** Input, int* Dims, double Min, double Max);

			void Copy1D(double* Input, double* Output, int Dim);
			void Copy3D(double*** Input, double*** Output, int* Dims);

			void ConvertTo1D(double*** Input, double* Output, int* Dims);
			void ConvertTo3D(double* Input, double*** Output, int* Dims);

			void Compare1D(double* Input1, double* Input2, int Dim, double Margin);
			void Compare3D(double*** Input1, double*** Input2, int* Dims, double Margin);

			void Pad(double*** Input, double*** Output, int* Dims, char Padding);
			void Flip(double*** Input, double*** Output, int* Dims);

			void Free1D(double* Input);
			void Free3D(double*** Input);

#endif
