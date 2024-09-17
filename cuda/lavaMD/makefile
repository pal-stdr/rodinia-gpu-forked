include ../../common/make.config

# Example
# target: dependencies
	# command 1
	# command 2
          # .
          # .
          # .
	# command n

ifdef OUTPUT
override OUTPUT = -DOUTPUT
endif

# C_C = gcc
# C_C = $(CC)
# OMP_LIB = -lgomp
# OMP_FLAG = -fopenmp

# CUD_C = $(NVCC)
# OMP_FLAG = 	-Xcompiler paste_one_here
# CUDA_FLAG = -arch sm_13
# CUDA_FLAG = $(NVCC_FLAGS)

# link objects (binaries) together
lavaMD:		main.o \
			./util/num/num.o \
			./util/timer/timer.o \
			./util/device/device.o
	$(LINKER) $(KERNEL_DIM) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) main.o \
			./util/num/num.o \
			./util/timer/timer.o \
			./util/device/device.o \
			-o lavaMD

# compile function files into objects (binaries)
main.o:		main.h \
			main.cu \
			./kernel/kernel_gpu_cuda_wrapper.h \
			./kernel/kernel_gpu_cuda_wrapper.cu \
			./util/num/num.h \
			./util/num/num.c \
			./util/timer/timer.h \
			./util/timer/timer.c \
			./util/device/device.h \
			./util/device/device.cu
	$(NVCC)	$(KERNEL_DIM) $(NVCC_FLAGS) $(OUTPUT) main.cu \
			-c \
			-o main.o

# ./kernel/kernel_gpu_cuda_wrapper.o:	./kernel/kernel_gpu_cuda_wrapper.h \
# 									./kernel/kernel_gpu_cuda_wrapper.cu
# 	$(CUD_C) $(KERNEL_DIM)						./kernel/kernel_gpu_cuda_wrapper.cu \
# 									-c \
# 									-o ./kernel/kernel_gpu_cuda_wrapper.o \
# 									-O3 \
# 									$(CUDA_FLAG)

./util/num/num.o:	./util/num/num.h \
					./util/num/num.c
	$(CC) $(CC_FLAGS)	./util/num/num.c \
					-c \
					-o ./util/num/num.o


./util/timer/timer.o:	./util/timer/timer.h \
						./util/timer/timer.c
	$(CC) $(CC_FLAGS)	./util/timer/timer.c \
						-c \
						-o ./util/timer/timer.o
						

./util/device/device.o:	./util/device/device.h \
						./util/device/device.cu
	$(NVCC) $(NVCC_FLAGS)	./util/device/device.cu \
						-c \
						-o ./util/device/device.o
						

# delete all object and executable files
clean:
	rm	*.o \
		./kernel/*.o \
		./util/num/*.o \
		./util/timer/*.o \
		./util/device/*.o \
		lavaMD \
		result.txt
