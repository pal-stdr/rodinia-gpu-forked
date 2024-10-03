include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc
# INCLUDE := $(CUDA_DIR)/include

SRC = gaussian.cu
EXE = gaussian

$(EXE): gaussian.o
	$(LINKER) $(LINKER_FLAGS) gaussian.o -o $(EXE)

gaussian.o: $(SRC)
	$(NVCC) $(KERNEL_DIM) $(NVCC_FLAGS) -c $(SRC) -o gaussian.o

# clang: $(SRC)
# 	clang++ $(SRC) -o $(EXE) -I../util --cuda-gpu-arch=sm_20 \
# 		-L/usr/local/cuda/lib64 -lcudart_static -ldl -lrt -pthread -DTIMING

clean:
	rm gaussian *.o
