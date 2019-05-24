#include "../../CNNTests.h"

/*
        	File Structure

	1 - Global Variables 

	2 - Model Construction
		2.1 - Init
		2.2 - Free
		2.3 - AddBlock
		2.4 - AddLayers
			2.4.1 - Conv
			2.4.2 - Pool
			2.4.3 - Fcon
			2.4.4 - Acti
			2.4.5 - Drop

	3 - Pre Defined Models
		3.1 - AlexNet
		3.2 - VGG16

	4 - Print

	5 - Save

	6 - Load
*/

// 1 --- Create Network --- //

	static void CreateNetwork(Network* Net)
	{
		int Dims[3];
		Dims[0] = 1;
		Dims[1] = 28;
		Dims[2] = 28;

		int Freq = 200, Pip = 2, Bs = 128;
		char EFunc = MSE;
		double Lr = 0.1, Mom = 0.02;

		SetLMemFreq(Freq);
		SetDesignFreq(Freq);
		SetPipelining(Pip);

		SetErrorFunc(Net, EFunc);
		SetBatchSize(Net, Bs);
		SetLearningRate(Net, Lr);
		SetMomentum(Net, Mom);

		InitCNN(Net, Dims);

		int NKernels = 32;
		char KernelSize = 3, Stride = 1, Padding = 0, Parallelism = 1;
		char FilterSize = 2, Type = 1;
		char Func = 2;
		int OutputSize = 100;
		double DropP = 0.5;

		AddBlock(Net);
		AddConv(NKernels, KernelSize, Stride, Padding, Parallelism);
		AddActi(Func);
		AddConv(NKernels, KernelSize, Stride, Padding, 2*Parallelism);
		AddPool(FilterSize, Type, 2*Stride, Parallelism);
		AddActi(Func);
		AddConv(NKernels, KernelSize, Stride, Padding, 120*Parallelism);
		AddActi(Func);
		AddConv(NKernels, KernelSize, Stride, Padding, 50*Parallelism);
		AddPool(FilterSize, 2*Type, 2*Stride, Parallelism);
		AddActi(Func);

		AddBlock(Net);
		AddFcon(OutputSize, Parallelism);
		AddActi(Func);
		AddDrop(DropP);
		AddFcon(10, Parallelism);
		AddActi(Soft);
	}

// 2 --- Model Construction --- //

	// 2.1 --- Custom --- //

		void CreateNetworkTest()
		{
			printf("Starting Create Network Test\n\n");

			Network* Net = malloc(sizeof(Network));

			CreateNetwork(Net);

			if(Debug)
			{
				PrintArchitecture(Net);
			}

			FreeCNN(Net);
			free(Net);

			printf("\nCreate Network Test Complete!\n\n");
		}

	// 2.2 --- AlexNet --- //

		void CreateAlexNetTest()
		{
			printf("Starting CreateAlexNet Test\n\n");

			Network* Net = malloc(sizeof(Network));

			CreateAlexNet(Net);

			if(Debug)
			{
				PrintArchitecture(Net);
			}

			FreeCNN(Net);
			free(Net);

			printf("\nCreate Network Test Complete!\n\n");
		}

	// 2.3 --- VGG16 --- //

		void CreateVGG16Test()
		{
			printf("Starting Create VGG16 Test\n\n");

			Network* Net = malloc(sizeof(Network));

			CreateVGG16(Net);

			if(Debug)
			{
				PrintArchitecture(Net);
			}

			FreeCNN(Net);
			free(Net);

			printf("\nCreate Network Test Complete!\n\n");
		}
