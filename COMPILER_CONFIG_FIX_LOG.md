# 1. Mother compiler config issue

## 1.1. `make clean` recursively delete core `Makefile`, `LICENSE` & `README`

### Why?
Because, by default `Makefile` cannot create `$(RODINIA_BASE_DIR)/bin/linux/cuda`, `$(RODINIA_BASE_DIR)/bin/linux/omp`, `$(RODINIA_BASE_DIR)/bin/linux/opencl` dirs. So everytime you run `make clean` command. When the `clean: CUDA_clean OMP_clean OCL_clean` target is called, it goes through every sub-target (e.g. `CUDA_clean`). And when it can't find the folders (e.g. `cd $(CUDA_BIN_DIR);`), it set the shell to current dir, and start removing `Makefile`, `LICENSE` & `README` randomly.

### Fix
Update the [Makefile](Makefile) like following

- Update every cleaning target from this

```Makefile
include ../../common/make.config

CUDA_clean:
	cd $(CUDA_BIN_DIR); rm -f *
	for dir in $(CUDA_DIRS) ; do cd cuda/$$dir ; make clean ; cd ../.. ; done
```

**To**

```Makefile
CUDA_clean:
	rm -rf $(CUDA_BIN_DIR)
	for dir in $(CUDA_DIRS) ; do cd cuda/$$dir ; make clean ; cd ../.. ; done
```

- Add the CUDA bin dir (i.e. `project-root/bin/linux/cuda`) creation target and hook it up with `CUDA:` target from this

```Makefile
CUDA: 
	cd cuda/backprop;		make;	cp backprop $(CUDA_BIN_DIR)
	cd cuda/bfs;			make;	cp bfs $(CUDA_BIN_DIR)
    # .....................
```

**To**

```Makefile
cuda_create_bin_dir:
	@if [ ! -d $(CUDA_BIN_DIR) ]; then \
		echo "Creating directory $(CUDA_BIN_DIR)"; \
		mkdir -p $(CUDA_BIN_DIR); \
	else \
		echo "Directory $(CUDA_BIN_DIR) already exists"; \
	fi;


# You have to use "@" in subcommands, to make it work properly
CUDA: cuda_create_bin_dir
	@cd cuda/backprop;		make;	cp backprop $(CUDA_BIN_DIR)
	@cd cuda/bfs;			make;	cp bfs $(CUDA_BIN_DIR)
    # .....................
```


## 1.2. Common config is needed inside [common/](common/)

- **All those configs are done in the shadow of `ivanradanov [rodinia's common dir](https://github.com/ivanradanov/rodinia/tree/cgo24/common) `cgo24` branch.**

- **Collect [cgo24/common/my_timing.h](https://github.com/ivanradanov/rodinia/tree/cgo24/common/), [cgo24/common/my_verification.h](https://github.com/ivanradanov/rodinia/tree/cgo24/common/my_verification.h) and add here  [`my_timing.h`](common/my_timing.h) & [`my_verification.h`](common/my_verification.h). They are for measuring time and result verification.**

### 1.2.1. Update [common/make.config](common/make.config)

Update the content to 

```Makefile
CUDA_VERSION := cuda-11.4


# Added by pal
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))


# CUDA toolkit installation path
CUDA_DIR = /usr/local/$(CUDA_VERSION)

# CUDA toolkit libraries
CUDA_LIB_DIR := $(CUDA_DIR)/lib
ifeq ($(shell uname -m), x86_64)
     ifeq ($(shell if test -d $(CUDA_DIR)/lib64; then echo T; else echo F; fi), T)
     	CUDA_LIB_DIR := $(CUDA_DIR)/lib64
     endif
endif

# CUDA SDK installation path
SDK_DIR = /usr/local/$(CUDA_DIR)/samples/


# include the timing header
FLAGS_TO_ADD = -I$(current_dir) -include my_timing.h -include my_verification.h


CC_FLAGS += $(FLAGS_TO_ADD)
CXX_FLAGS += $(FLAGS_TO_ADD)
NVCC_FLAGS += $(FLAGS_TO_ADD)




# ======== Conditionally load "nvcc.host.make.config" or "polygeist.host.make.config" =========

# Conditionally set COMPILER_NAME to nvcc if not defined by the user
# If You didn't specify "COMPILER_NAME=" by "make YOUR_TARGET COMPILER_NAME=" command, then by default, COMPILER_NAME= set to "nvcc"
COMPILER_NAME ?= nvcc

include $(current_dir)/$(COMPILER_NAME).host.make.config

# ============================================================================================




# define the compiler name
# Important for generating results "results/cuda/out/timestamp.log"
ifdef COMPILER_NAME
	FLAGS_TO_ADD += -D_MY_COMPILER_NAME_=\"$(COMPILER_NAME)\"
endif


MY_VERIFICATION_DISABLE = 1
ifneq ($(MY_VERIFICATION_DISABLE),0)
    FLAGS_TO_ADD += -DMY_VERIFICATION_DISABLE
endif
```


### 1.2.2. Add & define [common/nvcc.host.make.config](common/nvcc.host.make.config)

```Makefile
CC = /usr/local/$(CUDA_VERSION)/bin/nvcc
CXX = /usr/local/$(CUDA_VERSION)/bin/nvcc
NVCC = /usr/local/$(CUDA_VERSION)/bin/nvcc
LINKER = /usr/local/$(CUDA_VERSION)/bin/nvcc

# Set GPU arch targeted optimizations. If you don't want, set it empty
GPU_TARGETED_ARCH_FLAGS := -gencode arch=compute_86,code=sm_86

CC_FLAGS += -O3 -lgomp
CXX_FLAGS += -O3 -lgomp

# Set CUDA object compiler flags (i.e. optimizations)
NVCC_FLAGS += $(GPU_TARGETED_ARCH_FLAGS)
NVCC_FLAGS += -O3 -lgomp

# Set CUDA object linker flags (i.e. libs, optimizations, etc.)
LINKER_FLAGS += -lcudart -lcuda -lm
LINKER_FLAGS += -O3 -lgomp

CUDA_SAMPLES_PATH_ = /usr/local/$(CUDA_VERSION)/samples/
```


## 1.3. Backprop [cuda/backprop/Makefile](cuda/backprop/Makefile) fix

Keep in mind, for `$(NVCC_FLAGS)` (i.e. `GPU_TARGETED_ARCH_FLAGS := -gencode arch=compute_86,code=sm_86`)

- flags should be passed both to object generation + object linking phase.
- Only `*.cu` files need such arch specific flags.

```Makefile
include ../../common/make.config

backprop: backprop.o facetrain.o imagenet.o backprop_cuda.o 
	$(LINKER) $(LINKER_FLAGS) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) backprop.o facetrain.o imagenet.o backprop_cuda.o -o backprop

%.o: %.[ch]
	$(CC) $(CC_FLAGS) $< -c -o $@

facetrain.o: facetrain.c backprop.h
	$(CC) $(CC_FLAGS) facetrain.c -c -o $@
	
backprop.o: backprop.c backprop.h
	$(CC) $(CC_FLAGS) backprop.c -c -o $@

backprop_cuda.o: backprop_cuda.cu backprop.h
	$(NVCC) $(NVCC_FLAGS) -c backprop_cuda.cu -o $@

imagenet.o: imagenet.c backprop.h
	$(CC) $(CC_FLAGS) imagenet.c -c -o $@
```


## 1.4. BFS [cuda/bfs/Makefile](cuda/bfs/Makefile) fix

We donot need unnecessary targets like `release:`, `clang:`, `enum:`, `debug:`, `debugenum:`, `clean:`. So the clean `Makefile` is this

```Makefile
SRC = bfs.cu
OBJ = bfs.o
EXE = bfs.out


$(EXE): $(OBJ)
	$(LINKER) $(NVCC_FLAGS) $(OBJ) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@ -I../util

clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo result.txt *.o
```


## 1.5. CFD [cuda/cfd/Makefile](cuda/cfd/Makefile) fix

- Here we only need the `euler3d` bin. So we can comment out rest of the `euler3d_double`, `pre_euler3d`, `pre_euler3d_double` bin targets.


```Makefile
include ../../common/make.config

CUTIL_LIB = $(NVCC_FLAGS)
CUDA_SDK_PATH = $(CUDA_SAMPLES_PATH_)


all: euler3d # euler3d_double pre_euler3d pre_euler3d_double 

euler3d: euler3d.cu
	$(LINKER) $(KERNEL_DIM) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib $(CUTIL_LIB) euler3d.cu -o euler3d

# euler3d_double: euler3d_double.cu
# 	$(NVCC) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib $(CUTIL_LIB) euler3d_double.cu -o euler3d_double


# pre_euler3d: pre_euler3d.cu
# 	$(NVCC) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib $(CUTIL_LIB) pre_euler3d.cu -o pre_euler3d


# pre_euler3d_double: pre_euler3d_double.cu
# 	$(NVCC) -I$(CUDA_SDK_PATH)/common/inc -L$(CUDA_SDK_PATH)/lib $(CUTIL_LIB) pre_euler3d_double.cu -o pre_euler3d_double


clean:
	rm -f euler3d euler3d_double pre_euler3d pre_euler3d_double *.linkinfo
```

- And have to update [Makefile](Makefile) according to that.

```Makefile
CUDA: cuda_create_bin_dir
	# @cd cuda/cfd;				make;	cp euler3d euler3d_double pre_euler3d pre_euler3d_double $(CUDA_BIN_DIR)
	@cd cuda/cfd;				make;	cp euler3d $(CUDA_BIN_DIR)
```



## 1.6. GAUSSIAN [cuda/gaussian/Makefile](cuda/gaussian/Makefile) fix

```Makefile
# CC := $(CUDA_DIR)/bin/nvcc
# INCLUDE := $(CUDA_DIR)/include

SRC = gaussian.cu
EXE = gaussian

$(EXE): gaussian.o
	$(LINKER) $(NVCC_FLAGS) $(LINKER_FLAGS) gaussian.o -o $(EXE)

gaussian.o: $(SRC)
	$(NVCC) $(KERNEL_DIM) $(NVCC_FLAGS) -c $(SRC) -o gaussian.o

# clang: $(SRC)
# 	clang++ $(SRC) -o $(EXE) -I../util --cuda-gpu-arch=sm_20 \
# 		-L/usr/local/cuda/lib64 -lcudart_static -ldl -lrt -pthread -DTIMING

clean:
	rm gaussian *.o
```


## 1.7. HOTSPOT [cuda/hotspot/Makefile](cuda/hotspot/Makefile) fix

```Makefile
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
```


## 1.8. HOTSPOT3D [cuda/hotspot3D/Makefile](cuda/hotspot3D/Makefile) fix

- By default this is not added. So we have to add it to [Makefile](Makefile)

```Makefile
# Add "hotspot3D" in the following dir list
CUDA_DIRS := backprop bfs cfd gaussian heartwall hotspot hotspot3D kmeans lavaMD leukocyte lud nn	nw srad streamcluster particlefilter pathfinder mummergpu

# Add under "CUDA:" target
CUDA: cuda_create_bin_dir
	@cd cuda/hotspot3D;			make;	cp 3D $(CUDA_BIN_DIR)
```

- Then update [cuda/hotspot3D/Makefile](cuda/hotspot3D/Makefile)

```Makefile
include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc
# INCLUDE := $(CUDA_DIR)/include

SRC = 3D.cu
OBJ = 3D.o
EXE = 3D

OUTPUT = *.out

# FLAGS = -g -G #-arch sm_20 --ptxas-options=-v

$(EXE): $(OBJ)
	$(LINKER) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) $(OBJ) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@ -I../util

# release: $(SRC)
# 	$(CC) $(KERNEL_DIM) $(FLAGS) $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# enum: $(SRC)
# 	$(CC) $(KERNEL_DIM) $(FLAGS) -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# debug: $(SRC)
# 	$(CC) $(KERNEL_DIM) $(FLAGS) -g $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# debugenum: $(SRC)
# 	$(CC) $(KERNEL_DIM) $(FLAGS) -g -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo $(OUTPUT) *.o
```



## 1.9. lavaMD [cuda/lavaMD/Makefile](cuda/lavaMD/Makefile) fix

- **Have to be very careful on this one. Polygeist Ivanov heavily updated the this kernel launch parameters.**

- **The performance result from this kernel is discussed heavily in Polygeist paper.**

- **[cuda/lavaMD/main.cu](cuda/lavaMD/main.cu) didn't exist with the core rodinia before. They've created a separate this [cuda/lavaMD/main.cu](cuda/lavaMD/main.cu) file separately. And then called the kernel `kernel_gpu_cuda<<<blocks, threads>>>` from that `.cu` file and put their own timer on around it. The timer function name is `MY_START_CLOCK(lavaMD, )`. I have manually commented out the time calls. You can find them in line `230` & `237`. And they are defined in here [`#define MY_START_CLOCK(APP_ID, CLOCK_ID)`](https://github.com/ivanradanov/rodinia/blob/a97759e748669518c385f6d1e71c4b59ab43b121/common/my_timing.h#L20)**

- **Also the have put their own verifier function named `MY_VERIFY_DOUBLE_CUSTOM(fv_cpu, dim_cpu.space_mem / sizeof(fp), 1e-13, 1)`. I have no idea what it does. I have commented this out. This is a macro you can find here [`#define MY_VERIFY_DOUBLE_CUSTOM(ARRAY_PTR, SIZE, C1, C2)`](https://github.com/ivanradanov/rodinia/blob/a97759e748669518c385f6d1e71c4b59ab43b121/common/my_verification.h#L46)**

- **Also in [cuda/lavaMD/main.cu](cuda/lavaMD/main.cu), they have multiple `main.h` call at line `35`, and `50`. It was giving error. I have commented one out.**

- **In [cuda/lavaMD/makefile](cuda/lavaMD/makefile), also lot of changes. I have removed `$(POLYGEIST_LLVM_STRUCT_ABI_0)` from [`$(NVCC_FLAGS) $(POLYGEIST_LLVM_STRUCT_ABI_0)`](https://github.com/ivanradanov/rodinia/blob/a97759e748669518c385f6d1e71c4b59ab43b121/cuda/lavaMD/makefile#L52) line. I found the assigned value to this `Makefile` variable is `POLYGEIST_LLVM_STRUCT_ABI_0 = --struct-abi=0` defined in Ivanov's [`rodinia/common/common.polygeist.host.make.config`](https://github.com/ivanradanov/rodinia/blob/a97759e748669518c385f6d1e71c4b59ab43b121/common/common.polygeist.host.make.config#L33).**


```Makefile
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
		lavaMD
```


## 1.10. lud [cuda/lud/cuda/Makefile](cuda/lud/cuda/Makefile) fix

- **Have to be careful about the `Makefile` location. This one is actually here [cuda/lud/cuda/Makefile](cuda/lud/cuda/Makefile).**

- **Confused About the usage of some `Makefile` variables. Kept active for now `CUFILES`, `CCFILES`**

```Makefile
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
	$(LINKER) $(NVCC_FLAGS)  $? -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJS) *.linkinfo
```



## 1.11. nn [cuda/nn/Makefile](cuda/nn/Makefile) fix

```Makefile
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
```



## 1.12. nw [cuda/nw/Makefile](cuda/nw/Makefile) fix

- **Actual binary name is `needle`.**

- **Not sure that targets, `clang`, `enum`, `debug`, `debugenum` are being used or not! I didn't see those targets are called while doing the compilation.**

```Makefile
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
```



## 1.13. srad_v1 [cuda/srad/srad_v1/makefile](cuda/srad/srad_v1/makefile) fix

- **Only `srad_v1` is used & fixed.**


```Makefile
include ../../../common/make.config

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


SRC = main.cu
OBJ = main.o
EXE = srad

$(EXE): $(OBJ)
	$(LINKER) $(OBJ) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -I../util -c $< -o $@


# delete all object files
clean:
	rm *.o $(EXE)
```




## 1.14. streamcluster [cuda/streamcluster/Makefile](cuda/streamcluster/Makefile) fix

- **Actual binary name is `sc_gpu`.**

```Makefile
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
	$(LINKER) $(NVCC_FLAGS) $(COBJS) $(CXXOBJS) $(CUOBJS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXECUTABLE)

clean:
	rm -f $(COBJS) $(CXXOBJS) $(CUOBJS) $(EXECUTABLE)
```



## 1.15. particlefilter [cuda/particlefilter/Makefile](cuda/particlefilter/Makefile) fix

- **Generate 2 cubins; `particlefilter_naive` & `particlefilter_float`**


```Makefile
include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc

# INCLUDE := $(CUDA_DIR)/include

# all: naive float

# naive: ex_particle_CUDA_naive_seq.cu
# 	$(CC) -I$(INCLUDE) -L$(CUDA_LIB_DIR) -lcuda -g -lm -O3 -use_fast_math -arch sm_13 ex_particle_CUDA_naive_seq.cu -o particlefilter_naive
	
# float: ex_particle_CUDA_float_seq.cu
# 	$(CC) -I$(INCLUDE) -L$(CUDA_LIB_DIR) -lcuda -g -lm -O3 -use_fast_math -arch sm_13 ex_particle_CUDA_float_seq.cu -o particlefilter_float

# clean:
# 	rm particlefilter_naive particlefilter_float


SRC = ex_particle_CUDA_naive_seq.cu
OBJ = ex_particle_CUDA_naive_seq.o
EXE = particlefilter_naive

SRC2 = ex_particle_CUDA_float_seq.cu
OBJ2 = ex_particle_CUDA_float_seq.o
EXE2 = particlefilter_float


all: $(EXE) $(EXE2)


$(EXE): $(OBJ)
	$(LINKER) $(NVCC_FLAGS) $(OBJ) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@

$(EXE2): $(OBJ2)
	$(LINKER) $(NVCC_FLAGS) $(OBJ2) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE2)

$(OBJ2): $(SRC2)
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@

clean:
	rm $(EXE) $(EXE2) *.o
```



## 1.16. pathfinder [cuda/pathfinder/Makefile](cuda/pathfinder/Makefile) fix

- **Not sure that targets, `clang`, `enum`, `debug`, `debugenum` are being used or not! I didn't see those targets are called while doing the compilation.**

```Makefile
include ../../common/make.config

# CC := $(CUDA_DIR)/bin/nvcc
# INCLUDE := $(CUDA_DIR)/include

SRC = pathfinder.cu
OBJ = pathfinder.o
EXE = pathfinder


$(EXE): $(OBJ)
	$(LINKER) $(OBJ) $(NVCC_FLAGS) -L$(CUDA_LIB_DIR) $(LINKER_FLAGS) -o $(EXE)

$(OBJ): $(SRC)
	$(NVCC) $(NVCC_FLAGS) -I../util -c $< -o $@

clean:
	rm -f $(EXE) $(OBJ)


# release:
# 	$(CC) $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# clang: $(SRC)
# 	clang++ $(SRC) -o $(EXE) -I../util --cuda-gpu-arch=sm_20 \
# 		-L/usr/local/cuda/lib64 -lcudart_static -ldl -lrt -pthread -DTIMING

# enum:
# 	$(CC) -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$$(CUDA_LIB_DIR) 

# debug:
# 	$(CC) -g $(SRC) -o $(EXE) -I$(INCLUDE) -L$$(CUDA_LIB_DIR) 

# debugenum:
# 	$(CC) -g -deviceemu $(SRC) -o $(EXE) -I$(INCLUDE) -L$(CUDA_LIB_DIR) 

# clean:
# 	rm -f pathfinder
```