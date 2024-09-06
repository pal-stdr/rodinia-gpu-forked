# 1. Mother compiler config issue

## 1.1. `make clean` recursively delete core `Makefile`, `LICENSE` & `README`

### Why?
Because, by default `Makefile` cannot create `$(RODINIA_BASE_DIR)/bin/linux/cuda`, `$(RODINIA_BASE_DIR)/bin/linux/omp`, `$(RODINIA_BASE_DIR)/bin/linux/opencl` dirs. So everytime you run `make clean` command. When the `clean: CUDA_clean OMP_clean OCL_clean` target is called, it goes through every sub-target (e.g. `CUDA_clean`). And when it can't find the folders (e.g. `cd $(CUDA_BIN_DIR);`), it set the shell to current dir, and start removing `Makefile`, `LICENSE` & `README` randomly.

### Fix
Update the [Makefile](Makefile) like following

- Update every cleaning target from this

```Makefile
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


## 1.2. Common config is needed [common/nvcc.host.make.config](common/nvcc.host.make.config)

And the content is

```Makefile
CC = /usr/local/$(CUDA_VERSION)/bin/nvcc
CXX = /usr/local/$(CUDA_VERSION)/bin/nvcc
NVCC = /usr/local/$(CUDA_VERSION)/bin/nvcc
LINKER = /usr/local/$(CUDA_VERSION)/bin/nvcc

# Set GPU arch targeted optimizations. If you don't want, set it empty
GPU_TARGETED_ARCH_FLAGS := -gencode arch=compute_86,code=sm_86

CC_FLAGS = -O3 -lgomp
CXX_FLAGS = -O3 -lgomp

# Set CUDA object compiler flags (i.e. optimizations)
NVCC_FLAGS += $(GPU_TARGETED_ARCH_FLAGS)
NVCC_FLAGS += -O3 -lgomp

# Set CUDA object linker flags (i.e. libs, optimizations, etc.)
LINKER_FLAGS += -lcudart -lcuda -lm
LINKER_FLAGS += -O3 -lgomp

CUDA_SAMPLES_PATH_ = /usr/local/$(CUDA_VERSION)/samples/

COMPILER_NAME = nvcc
```


## 1.3. Backprop [cuda/backprop/Makefile](cuda/backprop/Makefile) fix

Keep in mind, for `$(NVCC_FLAGS)` (i.e. `GPU_TARGETED_ARCH_FLAGS := -gencode arch=compute_86,code=sm_86`)

- flags should be passed both to object generation + object linking phase.
- Only `*.cu` files need such arch specific flags.

```Makefile
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


## 1.4. CFD [cuda/cfd/Makefile](cuda/cfd/Makefile) fix

- Here we only need the `euler3d` bin. So we can comment out rest of the `euler3d_double`, `pre_euler3d`, `pre_euler3d_double` bin targets.


```Makefile
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



## 1.5. GAUSSIAN [cuda/gaussian/Makefile](cuda/gaussian/Makefile) fix

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


## 1.6. HOTSPOT [cuda/hotspot/Makefile](cuda/hotspot/Makefile) fix

```Makefile
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


## 1.7. HOTSPOT3D [cuda/hotspot3D/Makefile](cuda/hotspot3D/Makefile) fix

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