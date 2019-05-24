#include "../../CNN.h"

/*
	Load MNIST Dataset According to 
	https://pjreddie.com/projects/mnist-in-csv/

			File Structure

	1 - Data
		1.1 - Load
		1.2 - Free

*/

// 1 --- Data --- //

	// 1.1 --- Load --- //

		/*
        	Loads chosen DataSet

        	XTrain - Address of Training Data
        	YTrain - Address of Training Labels
        	XTest - Address of Test Data
        	YTest - Address of Test Labels
			Split - % of DataSet to be used for Testing. 0.4 means 40% will be for Test and 60% will be for Training
			DataSet - Which DataSet to Load

            Return Value - Nothing
        */
		
		void LoadData(double***** XTrain, double*** YTrain, double***** XTest, double*** YTest, double Split, char DataSet)
		{
			switch(DataSet)
			{
				case MNIST:
							LoadDataMNIST(XTrain, YTrain, XTest, YTest, Split);
							break;
			}
		}

	// 1.2 --- Free --- //

		/*
        	Frees chosen DataSet

        	XTrain - Training Data
        	YTrain - Training Labels
        	XTest - Test Data
        	YTest - Test Labels
			DataSet - Which DataSet to Free

            Return Value - Nothing
        */

		void FreeData(double**** XTrain, double** YTrain, double**** XTest, double** YTest, char DataSet)
		{
			switch(DataSet)
			{
				case MNIST:
							FreeDataMNIST(XTrain, YTrain, XTest, YTest);
							break;
			}
		}
