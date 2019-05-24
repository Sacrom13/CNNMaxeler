.PHONY: clean

CC=gcc
CFLAGS=-g -lm -lc -std=c99 -Wextra -I.
DEPS = CPUCode/Includes/CNN/CNN.h
OBJ = CPUCode/Main/MovingAverageSimpleCpuCode.c \
	\
	\
      CPUCode/Includes/CNN/Libs/DataManagement/Source/DataManagement.o CPUCode/Includes/CNN/Libs/DataManagement/Tests/TestSource/DataManagementTests.o \
      CPUCode/Includes/CNN/Libs/Debugging/Source/Debugging.o CPUCode/Includes/CNN/Libs/Debugging/Tests/TestSource/DebuggingTests.o \
      CPUCode/Includes/CNN/Libs/Timing/Source/Timing.o CPUCode/Includes/CNN/Libs/Timing/Tests/TestSource/TimingTests.o \
      \
      \
      CPUCode/Includes/CNN/Source/DataSets/LoadData.o CPUCode/Includes/CNN/Source/DataSets/MNIST/MNIST.o CPUCode/Includes/CNN/Tests/TestSource/DataSets/DataSetTests.o\
      CPUCode/Includes/CNN/Source/ErrorFuncs/ErrorFuncs.o CPUCode/Includes/CNN/Tests/TestSource/ErrorFuncs/ErrorFuncTests.o \
      CPUCode/Includes/CNN/Source/Layers/Conv.o CPUCode/Includes/CNN/Source/Layers/Fcon.o CPUCode/Includes/CNN/Source/Layers/Pool.o CPUCode/Includes/CNN/Tests/TestSource/Layers/LayerTests.o \
      \
      \
      CPUCode/Includes/CNN/Source/Models/Models.o CPUCode/Includes/CNN/Tests/TestSource/Models/ModelTests.o \
      \
      \
      CPUCode/Includes/CNN/Source/Network/Network.o CPUCode/Includes/CNN/Tests/TestSource/Network/NetworkTests.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

MovingAverageSimpleCPUCode: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS)

clean:
	rm MovingAverageSimpleCPUCode; find . -name '*.o' -delete
