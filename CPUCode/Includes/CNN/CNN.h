#ifndef CNN_DEFINED
#define CNN_DEFINED

	// 1 --- CNN Libs --- //
	
		#include "Libs/CNNLibs.h"
	
	// 2 --- Extra Libs --- //

		#include "Source/DataSets/DataSets.h"
		#include "Source/ErrorFuncs/ErrorFuncs.h"
		#include "Source/Layers/Layers.h"
		#include "Source/Network/Network.h"
		#include "Source/Models/Models.h"

#endif

// DOCS
/*
	// Overall 
	http://cs231n.github.io/convolutional-networks/#conv
	https://adeshpande3.github.io/A-Beginner%27s-Guide-To-Understanding-Convolutional-Neural-Networks-Part-2/
	
	// Conv Layer
	https://medium.com/@2017csm1006/forward-and-backpropagation-in-convolutional-neural-network-4dfa96d7b37e
	https://towardsdatascience.com/types-of-convolutions-in-deep-learning-717013397f4d

	// SoftMax Forward and BackProp
		- Others
			https://deepnotes.io/softmax-crossentropy
			
		- Current

			// CrossEnt
			https://www.ics.uci.edu/~pjsadows/notes.pdf

			// MSE
			https://www.ics.uci.edu/~pjsadows/notes.pdf
			Switch derivative for MSE derivative.

	// Printf Cursor
	\33[2K  - erases the entire line your cursor is currently on
	\033[A  - moves your cursor up one line, but in the same column i.e. not to the start of the line
	\033[F  - moves cursor up one line, and goes to the beggining ( does not erease)
	\r      - brings your cursor to the beginning of the line (r is for rewind) but does not erase anything

	// Directories
	Get Current Directory :

	char cwd[1000];
	if(getcwd(cwd, sizeof(cwd)) != NULL)
	{
	    printf("Current working dir: %s\n", cwd);
	}




 	CONV KERNEL
 						Burst size 3, BurstMult = 1, LayerPara = 2


											Input(4x3x3)

									x00 x01 x02		x09 x10 x11
									x03 x04 x05		x12 x13 x14
									x06 x07 x08		x15 x16 x17

									x18 x19 x20		x27 x28 x29
									x21 x22 x23		x30 x31 x32
									x24 x25 x26		x33 x34 x35


						4 Kernels(4x2x2)


					k00 k01		k04 k05						k16	k17		k20	k21
					k02 k03		k06	k07						k18	k19		k22	k23
		Kernel 1								Kernel 2
					k08	k09		k12	k13						k24	k25		k28	k29
					k10	k11		k14	k15						k26	k27		k30	k31



					k32 k33		k36 k37						k48	k49		k52	k53
					k34 k35		k38	k39						k50	k51		k54	k55
		Kernel 3								Kernel 4
					k40	k41		k44	k45						k56	k57		k60	k61
					k42	k43		k46	k47						k58	k59		k62	k63



		n				When to Output											First Output Point


		0			output on x09, x10 and x12.  									( 0 )
		1			output on tick x13, x(9 + 36) and x(10 + 36).  					( 3 )
		2			output on tick x12, x13, x(9 + 36).								( 2 )
		3			output on tick x10, x12, x13.									( 1 )
		4			output on tick x09, x10, x12									( 0 )
		5			output on tick x13, x14(pad), x15(pad)							( 3 )


			Only Output if:
							BurstSize * BurstMult * CurrentCall < NOutputPoints < BurstSize * BurstMult * (CurrentCall + 1)
							CTicks = InDims[0] / Parallelism			( Has to be Integer )
							YTicks <= OutDims[1
							XTicks <= OutDims[1]

			LMem Syncing takes precendence over Ticks.
			Need go make Ticks go through entire image and can't use controlled inputs in order for Stream.offset() to work properly

			Calculate the First Output Point, given by the following Formula (FirstOutputPoint[0] = 0):
			FirstOutputPoint[n] = (FirstOutputPoint[n - 1] + (BurstSize * BurstMult)) % (OutDims[1] * OutDims[1])
*/
