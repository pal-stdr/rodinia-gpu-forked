include ../../common/make.config

# NVCC = $(CUDA_DIR)/bin/nvcc

# NVCC_FLAGS = -I$(CUDA_DIR)/include

# TARGET_G = sc_gpu


# # make dbg=1 tells nvcc to add debugging symbols to the binary
# ifeq ($(dbg),1)
# 	NVCC_FLAGS += -g -O0
# else
# 	NVCC_FLAGS += -O3
# endif

# # make emu=1 compiles the CUDA kernels for emulation
# ifeq ($(emu),1)
# 	NVCC_FLAGS += -deviceemu
# endif

# # make dp=1 compiles the CUDA kernels with double-precision support
# ifeq ($(dp),1)
# 	NVCC_FLAGS += --gpu-name sm_13
# endif


# $(TARGET_G): streamcluster_cuda_cpu.cpp streamcluster_cuda.cu streamcluster_header.cu
# 	$(NVCC) $(NVCC_FLAGS) streamcluster_cuda_cpu.cpp streamcluster_cuda.cu streamcluster_header.cu -o $(TARGET_G) -lcuda
	

# clean:
# 	rm -f *.o *~ *.txt $(TARGET_G) *.linkinfo



EXECUTABLE := sc_gpu
CFILES :=
CXXFILES := streamcluster_cuda_cpu.cpp
CUFILES := streamcluster_cuda.cu #streamcluster_header.cu
COBJS=$(CFILES:.c=.c.o)
CXXOBJS=$(CXXFILES:.cpp=.cpp.o)
CUOBJS=$(CUFILES:.cu=.cu.o)

OUTPUT =

.SUFFIXES: .c.o .cpp.o .cu.o .cu

%.c.o: %.c
	$(CC) $(OUTPUT) $(CC_FLAGS) -c $< -o $@

%.cu.o: %.cu
	$(NVCC) $(OUTPUT) $(NVCC_FLAGS) -c $< -o $@

%.cpp.o: %.cpp
	$(CXX) $(OUTPUT) $(CXX_FLAGS) -c $< -o $@

$(EXECUTABLE): $(COBJS) $(CXXOBJS) $(CUOBJS)
	$(LINKER) $(COBJS) $(CXXOBJS) $(CUOBJS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXECUTABLE)

clean:
	rm -f $(COBJS) $(CXXOBJS) $(CUOBJS) $(EXECUTABLE) output.txt
