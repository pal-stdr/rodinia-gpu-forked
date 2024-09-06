include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc
# INCLUDE := $(CUDA_DIR)/include

SRC = hotspot.cu
OBJ = hotspot.o
EXE = hotspot

$(EXE): $(OBJ)
	$(LINKER) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) $(OBJ) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@ -I../util

# enum: $(SRC)
# 	$(CC) $(KERNEL_DIM) -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# debug: $(SRC)
# 	$(CC) $(KERNEL_DIM) -g $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# debugenum: $(SRC)
# 	$(CC) $(KERNEL_DIM) -g -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo result.txt *.o
