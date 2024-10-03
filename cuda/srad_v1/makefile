include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc

# INCLUDE := $(CUDA_DIR)/include

# # Example
# # target: dependencies
# 	# command 1
# 	# command 2
#           # .
#           # .
#           # .
# 	# command n

# # link objects(binaries) together
# a.out:		main.o
# 	$(CC)	main.o \
# 				-I$(INCLUDE) \
# 				-L$(CUDA_LIB_DIR)  \
# 				-lm -lcuda -lcudart -o srad

# # compile main function file into object (binary)
# main.o: 	main.cu \
# 				define.c \
# 				graphics.c \
# 				extract_kernel.cu \
# 				prepare_kernel.cu \
# 				reduce_kernel.cu \
# 				srad_kernel.cu \
# 				srad2_kernel.cu \
# 				compress_kernel.cu
# 	nvcc	main.cu \
# 				-c -O3 -arch sm_35

# # delete all object files
# clean:
# 	rm *.o srad


# SRC = main.cu compress_kernel.cu define.c device.c extract_kernel.cu graphics.c prepare_kernel.cu reduce_kernel.cu resize.c srad_kernel.cu srad2_kernel.cu
SRC = main.cu
OBJ = main.o
EXE = srad

$(EXE): $(OBJ)
	$(LINKER) $(OBJ) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -I../util -c $< -o $@


# delete all object files
clean:
	rm *.o $(EXE)