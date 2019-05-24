# Array Math
import numpy as np

# Keras
import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import Activation
from keras import layers

# To Deal with Randomness
from numpy.random import seed
from tensorflow import set_random_seed


def WriteWeights(Model, InputSize, OutputSize):
	Weights = Model.get_weights()[0]

	file = open("TestWeights.txt", 'w+')

	for y in range(InputSize):
		for x in range(OutputSize):
			file.write("%.4f " % Weights[y][x])
		file.write("\n")
	file.write("\n")	

def WriteTestData(InputSize):

	X = np.random.rand(InputSize)

	file = open("TestData.txt", 'w+')

	for i in range(InputSize):
		file.write("%.4f " % X[i])
	file.write("\n")

	return X

def WriteOutput(Model, TestData):

	InputData = np.array([TestData])
	
	X = Model.predict(InputData)

	file = open("TestOutput.txt", 'w+')

	for i in range(X.shape[1]):
		file.write("%.4f " % X[0][i])
	file.write("\n")

def CreateModel(InputSize, OutputSize, Act):
	
	Model = Sequential()
	Model.add(Dense(OutputSize, activation = Act, use_bias = False, input_dim = InputSize))

	Model.compile(optimizer='rmsprop', loss='categorical_crossentropy', metrics=['accuracy'])

	return Model

def main():
	seed(4)
	set_random_seed(5)

	NChannels = 1
	InputSize = 128
	OutputSize = 256
	Act = 'relu'

	Model = CreateModel(InputSize, OutputSize, Act)
	WriteWeights(Model, InputSize, OutputSize)
	TestData = WriteTestData(InputSize)
	WriteOutput(Model, TestData)

if __name__== "__main__":
  main()