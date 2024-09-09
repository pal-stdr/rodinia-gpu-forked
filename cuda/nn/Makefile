include ../../common/make.config

# LOCAL_CC = gcc -g -O3 -Wall
# CC := $(CUDA_DIR)/bin/nvcc

SRC = nn_cuda.cu
OBJ = nn_cuda.o
EXE = nn

# all : nn hurricane_gen
all : nn

# clean :
# 	rm -rf *.o nn hurricane_gen

# nn : nn_cuda.cu
# 	$(CC) -cuda nn_cuda.cu
# 	$(CC) -o nn nn_cuda.cu

# clang: $(SRC)
# 	clang++ nn_cuda.cu -o nn -I../util --cuda-gpu-arch=sm_20 \
# 		-L/usr/local/cuda/lib64 -lcudart_static -ldl -lrt -pthread -DTIMING

# hurricane_gen : hurricane_gen.c
# 	$(LOCAL_CC) -o $@ $< -lm


$(EXE): $(OBJ)
	$(LINKER) $(OBJ) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@

clean :
	rm -rf *.o $(EXE)

#data :
#	mkdir data
#	./gen_dataset.sh
