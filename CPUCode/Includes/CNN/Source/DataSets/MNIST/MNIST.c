#include "../../../CNN.h"

/*
	Load MNIST Dataset According to 
	https://pjreddie.com/projects/mnist-in-csv/

			File Structure

	1 - Global Variables

	2 - Data
		2.1 - Load
		2.2 - Free

*/

// 1 --- Global Variables --- //

	static double SplitStore;	// Store the Split for Free Later

// 2 --- Data --- //

	// 2.1 --- Load --- //

		/*
        	Loads MNIST Train Data

        	XTrain - Address of Training Data ( Dimensions {70000 * (1 - split), 1, 28, 28} )
        	YTrain - Address of Training Labels ( Dimensions {70000 * (1 - split), 10} )
        	XTest - Address of Test Data ( Dimensions {70000 * split, 1, 28, 28} )
        	YTest - Address of Test Labels ( Dimensions {70000 * split, 10} )
			Split - % of DataSet to be used for Testing. 0.4 means 40% will be for Test and 60% will be for Training

            Return Value - Nothing
        */

		void LoadDataMNIST(double***** XTrain, double*** YTrain, double***** XTest, double*** YTest, double Split)
		{
			SplitStore = Split;

			int DataSize = 70000;
			int InputChannels = 1;
			int InputSize = 28;
			int OutputSize = 10;

			int ImgDims[3];
			ImgDims[0] = InputChannels;
			ImgDims[1] = InputSize;
			ImgDims[2] = InputSize;

			FILE * File = fopen("CPUCode/Includes/CNN/Source/DataSets/MNIST/MNIST.csv", "r");

			if(File == NULL)
			{
				char cwd[1000];
				if(getcwd(cwd, sizeof(cwd)) != NULL)
				{
				    printf("Current working dir: %s\n", cwd);
				}
				printf("Error Opening File!\n");
				exit(-1);
			}

			int c;
			int CurrentSample = 0;
			char Flag = 1;
			int y = 0, x = 0;

			double Data = 0;

			int SplitSample = round(DataSize * Split);

			*XTrain = malloc(sizeof(double***) * round(DataSize * (1 - Split)) );
			*YTrain = malloc(sizeof(double*) * round(DataSize * (1 - Split)) );

			*XTest = malloc(sizeof(double***) * SplitSample);
			*YTest = malloc(sizeof(double*) * SplitSample);
			(*XTest)[CurrentSample] = Init3D(ImgDims);
			(*YTest)[CurrentSample] = Init1D(OutputSize);

			while((c = fgetc(File)) != EOF)
			{
				char test = (char) c;

				// Label
				if(Flag == 1)
				{
					if(CurrentSample < SplitSample)
					{
						(*YTest)[CurrentSample][atoi(&test)] = 1;
					}
					else
					{
						(*YTrain)[CurrentSample - SplitSample][atoi(&test)] = 1;
					}
					
					Flag = 2;
					continue;
				}

				// Comma Means Number is over
		    	if(test == ',')
		    	{
		    		if(Flag == 2)
		    		{
		    			Flag = 0;
		    			continue;
		    		}

		    		if(CurrentSample < SplitSample)
		    		{
		    			(*XTest)[CurrentSample][0][y][x] = Data;
		    		}
		    		else
		    		{
		    			(*XTrain)[CurrentSample - SplitSample][0][y][x] = Data;
		    		}

		    		++x;
		    		if(x == InputSize)
		    		{
		    			x = 0;
		    			++y;
		    		}

		    		Data = 0;
		    		continue;
		    	}

				// New Sample
		    	if(test == '\n')
		    	{
		    		if(CurrentSample < SplitSample)
		    		{
		    			(*XTest)[CurrentSample][0][y][x] = Data;
		    		}
		    		else
		    		{
		    			(*XTrain)[CurrentSample - SplitSample][0][y][x] = Data;
		    		}

		    		x = 0;
		    		y = 0;
		    		Data = 0;

		    		printf("Loading Data, %d/%d\r", CurrentSample, DataSize);

		    		Flag = 1;
		    		++CurrentSample;
		    		
		    		if(CurrentSample < SplitSample)
					{
						(*XTest)[CurrentSample] = Init3D(ImgDims);
						(*YTest)[CurrentSample] = Init1D(OutputSize);
					}
					else
					{
						if(CurrentSample == DataSize)
						{
							continue;
						}
						(*XTrain)[CurrentSample - SplitSample] = Init3D(ImgDims);
						(*YTrain)[CurrentSample - SplitSample] = Init1D(OutputSize);
					}
		    	}

		    	// Data Numbers
		    	else
		    	{
					Data *= 10;
					Data += atoi(&test);
		    	}
			}

			printf("\33[2KData Loaded!\n");
			fclose(File);
		}

	// 1.1.2 --- Free --- //

		/*
        	Frees MNIST Data

            Return Value - Nothing
        */

		void FreeDataMNIST(double**** XTrain, double** YTrain, double**** XTest, double** YTest)
		{
			int DataSize = 70000;
			int SplitSample = round(DataSize * SplitStore);

			for(int i = 0; i < DataSize; ++i)
			{
				if(i < SplitSample)
				{
					Free3D(XTest[i]);
					Free1D(YTest[i]);
				}
				else
				{
					Free3D(XTrain[i - SplitSample]);
					Free1D(YTrain[i - SplitSample]);
				}					
			}

			free(XTrain);
			free(YTrain);
			free(XTest);
			free(YTest);
		}
