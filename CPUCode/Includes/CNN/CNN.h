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

		/*#include "Maxfiles.h" 			// Includes .max files
		#include <MaxSLiCInterface.h>	// Simple Live CPU interface*/

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
*/
