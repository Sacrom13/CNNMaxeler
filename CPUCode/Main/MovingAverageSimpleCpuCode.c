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
#include "../Includes/CNN/Tests/CNNTests.h"//

// ---------------------------------------------------------- //
//                  2 -  Main				                  //
// ---------------------------------------------------------- //

void CreateTestNetwork(Network* Net, int* InDims)
{
	InitCNN(Net, InDims);

	AddBlock(Net);
	AddConv(16, 3, 2, 2, 1);
	AddActi(ReLu);
	AddConv(16, 3, 1, 0, 4);
	AddActi(ReLu);
	AddPool(2, MaxPool, 2, 1);
	/*AddFcon(1000, 1);
	AddActi(ReLu);*/

	/*InitCNN(Net, InDims);

	AddBlock(Net);
	AddConv(64, 3, 1, 1, 1);
	AddActi(ReLu);
	AddConv(64, 3, 1, 1, 1);
	AddActi(ReLu);
	AddPool(2, MaxPool, 2, 1);*/

	AddBlock(Net);
	AddFcon(100, 1);
	AddActi(ReLu);
	AddFcon(100, 1);
	AddActi(Soft);

	//SetBurstMult(Net, 0, 2048);

	SetBurstMult(Net, 0, 3);
}

int main()
{
	ToggleDebugMode();

	Network* Net = malloc(sizeof(Network));

	printf("Setting Network\n");
	int InDims[3] = {3, 29, 29};

	CreateTestNetwork(Net, InDims);
	//CreateVGG16(Net);

	double*** Input = Init3D(InDims);
	RandomizeArray3D(Input, InDims, 0, 5);

	printf("Network Set!\n");

	CNNForwardDFE(*Net, Input);

	Free3D(Input);
	FreeCNN(Net);
	free(Net);

	return 0;
}
