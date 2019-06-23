/**
 * Document: MaxCompiler Tutorial (maxcompiler-tutorial)
 * Chapter: 3      Example: 1      Name: Moving Average Simple
 * MaxFile name: MovingAverageSimple
 * Summary:
 *   CPU code for the three point moving average design.
 */

// ---------------------------------------------------------- //
//                   1 - CNN Imports                          //
// ---------------------------------------------------------- //

#include "../Includes/CNN/CNN.h"

// ---------------------------------------------------------- //
//                  2 -  Main				                  //
// ---------------------------------------------------------- //

void CreateTestNetwork(Network* Net, int* InDims)
{
	InitCNN(Net, InDims);

	AddBlock(Net);
	AddConv(16, 3, 1, 0);
	AddActi(ReLu);
	AddConv(16, 3, 1, 0);
	AddActi(Sigmoid);
	AddPool(2, MaxPool, 2);

	AddBlock(Net);
	AddFcon(100);
	AddActi(ReLu);
	AddFcon(100);
	AddActi(Sigmoid);
	AddFcon(100);
	AddActi(Tanh);

	/*InitCNN(Net, InDims);

	AddBlock(Net);
	AddConv(64, 3, 1, 1);
	AddActi(ReLu);
	AddConv(64, 3, 1, 1);
	AddActi(ReLu);
	AddPool(2, MaxPool, 2);*/

	AddBlock(Net);
	AddFcon(100);
	AddActi(ReLu);
	AddFcon(100);
	AddActi(Soft);
}

int main()
{
	ToggleDebugMode();

	Network* Net = malloc(sizeof(Network));

	printf("Setting Network\n");
	int InDims[3] = {3, 30, 30};

	CreateTestNetwork(Net, InDims);

	int** BurstMult = malloc(Net->TotalBlocks * sizeof(int*));
	int** ForwParallelism = malloc(Net->TotalBlocks * sizeof(int*));
	int** BackParallelism = malloc(Net->TotalBlocks * sizeof(int*));
	for(int i = 0; i < Net->TotalBlocks; ++i)
	{
		BurstMult[i] = malloc(Net->Blocks[i].BlockSize * sizeof(int));
		ForwParallelism[i] = malloc(Net->Blocks[i].BlockSize * sizeof(int));
		BackParallelism[i] = malloc(Net->Blocks[i].BlockSize * sizeof(int));
		for(int j = 0; j < Net->Blocks[i].BlockSize; ++j)
		{
			BurstMult[i][j] = 1;
			ForwParallelism[i][j] = 1;
			BackParallelism[i][j] = 1;
		}
	}
	BurstMult[0][0] = 29;
	BurstMult[0][1] = 25;
	BurstMult[0][2] = 113;

	BurstMult[1][0] = 1;
	BurstMult[1][1] = 1;
	BurstMult[1][2] = 1;

	DFECompile(Net, BurstMult, ForwParallelism, BackParallelism);

	printf("Network Set!\n");

	double*** Input = Init3D(InDims);
	RandomizeArray3D(Input, InDims, 0, 5);

	CNNForwardDFE(*Net, Input);

	Free3D(Input);
	FreeCNN(Net);
	free(Net);

	return 0;
}
