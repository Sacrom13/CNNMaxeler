# Array Math
import numpy as np

# Keras
import keras
from keras.models import Sequential
from keras.layers import MaxPooling2D

# To Deal with Randomness
from numpy.random import seed
from tensorflow import set_random_seed

def WriteTestData(InDims):

	X = np.random.rand(InDims[0], InDims[1], InDims[2])

	file = open("TestData.txt", 'w+')

	for channel in range(InDims[2]):
		for y in range(InDims[1]):
			for x in range(InDims[0]):
				file.write("%.4f " % X[x][y][channel])
		file.write("\n")

	return X

def WriteOutput(Model, TestData):

	InputData = np.array([TestData])
	
	X = Model.predict(InputData)

	file = open("TestOutput.txt", 'w+')

	for channel in range(X.shape[3]):
		for y in range(X.shape[2]):
			for x in range(X.shape[1]):
				file.write("%.4f " % X[0][x][y][channel])
		file.write("\n")

def CreateModel(InDims, FilterSize, Stride):
	
	Model = Sequential()
	Model.add(MaxPooling2D(pool_size=(FilterSize, FilterSize), strides = Stride, input_shape = InDims))

	Model.compile(optimizer='rmsprop', loss='categorical_crossentropy', metrics=['accuracy'])

	return Model

def main():
	seed(2)
	set_random_seed(3)

	NChannels = 3
	InputSize = 28
	FilterSize = 2
	Stride = 1

	InDims = (InputSize, InputSize, NChannels)

	Model = CreateModel(InDims,FilterSize, Stride)
	TestData = WriteTestData(InDims)
	WriteOutput(Model, TestData)

if __name__== "__main__":
  main()