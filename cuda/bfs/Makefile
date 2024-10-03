include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc
# INCLUDE := $(CUDA_DIR)/include


SRC = bfs.cu
OBJ = bfs.o
EXE = bfs.out


$(EXE): $(OBJ)
	$(LINKER) $(OBJ) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@ -I../util

clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo result.txt *.o
