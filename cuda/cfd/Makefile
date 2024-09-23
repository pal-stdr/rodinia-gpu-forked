include ../../common/make.config

# CUDA_SDK_PATH := $(SDK_DIR)

# # Determine the correct version of the cutil library
# CUTIL_LIB = # -lcutil
# ifeq ($(shell uname -m), x86_64)
#      ifeq ($(shell if test -e $(SDK_DIR)/lib/libcutil_x86_64.a; then echo T; else echo F; fi), T)
#         CUTIL_LIB = #-lcutil_x86_64
#      endif
# endif


CUTIL_LIB = $(NVCC_FLAGS)
CUDA_SDK_PATH = $(CUDA_SAMPLES_PATH)




all: euler3d # euler3d_double pre_euler3d pre_euler3d_double 

euler3d: euler3d.o
	$(LINKER) $(NVCC_FLAGS) $(LINKER_FLAGS) -L$(CUDA_SDK_COMMON_LIB_PATH) -L$(CUDA_LIB_DIR) euler3d.o -o euler3d

euler3d.o: euler3d.cu
	$(NVCC) $(KERNEL_DIM) $(NVCC_FLAGS) -I ../hybridsort -I$(CUDA_SDK_COMMON_INCLUDE_PATH) -c euler3d.cu -o euler3d.o

# euler3d: euler3d.cu
# 	$(LINKER) $(KERNEL_DIM) -I$(CUDA_SDK_COMMON_INCLUDE_PATH) $(NVCC_FLAGS) -L$(CUDA_SDK_COMMON_LIB_PATH) -L$(CUDA_LIB_DIR) $(CUTIL_LIB) euler3d.cu -o euler3d

# euler3d_double: euler3d_double.cu
# 	$(NVCC) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib -L$(CUDA_LIB_DIR) $(CUTIL_LIB) euler3d_double.cu -o euler3d_double


# pre_euler3d: pre_euler3d.cu
# 	$(NVCC) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib -L$(CUDA_LIB_DIR) $(CUTIL_LIB) pre_euler3d.cu -o pre_euler3d


# pre_euler3d_double: pre_euler3d_double.cu
# 	$(NVCC) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib -L$(CUDA_LIB_DIR) $(CUTIL_LIB) pre_euler3d_double.cu -o pre_euler3d_double


clean:
	rm -f euler3d euler3d_double pre_euler3d pre_euler3d_double *.o *.linkinfo
