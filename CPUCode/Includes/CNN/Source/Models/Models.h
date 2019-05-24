#ifndef MODELS_DEFINED
#define MODELS_DEFINED

	// 1 --- Required Libs --- //

		#include "../../Libs/CNNLibs.h"

	// 2 --- Function Prototypes --- //
		
		void InitCNN(Network* Net, int* InputDims);
		void FreeCNN(Network* Net);

		void AddBlock(Network* Net);
		void AddConv(int NKernels, char KernelSize, char Stride, char Padding, char Parallelism);
		void AddPool(char FilterSize, char Type, char Stride, char Parallelism);
		void AddFcon(int OutputSize, char Parallelism);
		void AddActi(char Func);
		void AddDrop(double DropP);

		void SetErrorFunc(Network* Net, char Func);
		void SetBatchSize(Network* Net, int Bs);
		void SetLearningRate(Network* Net, double Lr);
		void SetMomentum(Network* Net, double Mom);
		void SetBurstMult(Network* Net, int Block, int BM);

		void CreateVGG16(Network* Net);
		void CreateAlexNet(Network* Net);

		void PrintArchitecture(Network* Net);

#endif
