include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc
CC := $(NVCC)


# INCLUDE := $(CUDA_DIR)/include

SRC = needle.cu
OBJ = needle.o
EXE = needle

# release: $(SRC)
# 	$(CC) ${KERNEL_DIM} $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR)

$(EXE): $(OBJ)
	$(LINKER) $(OBJ) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -I../util -c $< -o $@

clang: $(SRC)
	clang++ $(SRC) -o $(EXE) -I../util --cuda-gpu-arch=sm_20 \
		-L/usr/local/cuda/lib64 -lcudart_static -ldl -lrt -pthread -DTIMING

enum: $(SRC)
	$(CC) ${KERNEL_DIM} -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

debug: $(SRC)
	$(CC) ${KERNEL_DIM} -g $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

debugenum: $(SRC)
	$(CC) ${KERNEL_DIM} -g -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo result.txt *.o
