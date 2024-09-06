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



## 1.2. Backprop [cuda/backprop/Makefile](cuda/backprop/Makefile) fix

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