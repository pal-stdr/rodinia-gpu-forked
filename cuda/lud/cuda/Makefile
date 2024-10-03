include ../../../common/make.config

# CC = gcc
# NVCC = nvcc

DEFS += \
		-DGPU_TIMER \
		$(SPACE)

NVCC_FLAGS += -I../common \
			 $(SPACE)

# CFLAGS += -I../common \
# 					-I/usr/include/cuda \
# 		  -O3 \
# 		  -Wall \
# 		  $(SPACE)

# Add source files here
EXECUTABLE  := lud_cuda
# Cuda source files (compiled with cudacc)
CUFILES     := lud_kernel.cu
# C/C++ source files (compiled with gcc / c++)
CCFILES     := lud.c lud_cuda.c ../common/common.c

OBJS = ../common/common.o lud.o lud_kernel.o

.PHONY: all clean 
all : $(EXECUTABLE)

.c.o : 
	$(CC) $(KERNEL_DIM) $(CC_FLAGS) $(DEFS) -c $< -o $@

%.o:	%.cu 
	$(NVCC) $(KERNEL_DIM) $(NVCC_FLAGS) $(DEFS) -c $< -o $@

# clang: $(SRC)
# 	clang++ lud.cu lud_kernel.cu ../common/common.c -o $(EXECUTABLE) \
# 		-I../common -I../../util --cuda-gpu-arch=sm_20 \
# 		-L/usr/local/cuda/lib64 -lcudart_static -ldl -lrt -pthread -DTIMING

$(EXECUTABLE) : $(OBJS)
	$(LINKER) $? -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJS) *.linkinfo
