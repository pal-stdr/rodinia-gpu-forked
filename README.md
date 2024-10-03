# Rodinia Benchmark for CUDA + Polygeist-GPU

- (The original rodinia readme is at [README](README))

- Benchmark in the light of `ivanradanov's` work [rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24), based on the paper [Retargeting and Respecializing GPU Workloads for Performance Portability](https://c.wsmoses.com/papers/polygeist24.pdf). The repo link is at the `cgo24` branch of [Polygeist](https://github.com/llvm/Polygeist/tree/cgo24)


# Acknowledgement

- Lot of code has been collected from [rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24). So atmost gratitude for them.


# Objectives

- Developing a benchmark for rodinia CUDA and Polygeist-GPU targeting CUDA.




# Pre-requisites





# How to use `nvcc` (i.e. just [cuda/](cuda/)) compilation flow?


## Pre-requisites for [cuda/](cuda/) Benchmark

- `CUDA-11.4` & `CUDA-12.2` is tested & used just for [cuda/](cuda/) benchmarks. Compiler expects that your std CUDA installation path is at `/usr/local/cuda-XX.X`. But if you have CUDA at other path, then specify with `make CUDA CUDA_INSTALL_PATH=/some/other/path/cuda-XX.X`

- **`CUDA-11.4` needed if you want to work with [ivanradanov's rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24)**. Because that benchmark was done with `CUDA-11.4`.

- For `COMPILER_NAME=nvcc` compiler config (i.e. [`common/nvcc.host.make.config`](common/nvcc.host.make.config)), you can use same or different CUDA version. Just you have update that config in respective `common/$(COMPILER_NAME).host.make.config` files.

- For general experiments, you should be able to use `CUDA-11.X` or `CUDA-12.X`.

- For CUDA specific compilation flag knowledge, jump to [Docs/CUDA/TARGETING_GPU_ARCH_with_COMPILATION_FLAGS.md](Docs/CUDA/TARGETING_GPU_ARCH_with_COMPILATION_FLAGS.md).

- If you see any `Makefile` variable is defined as `MAKE_VAR ?=` (e.g. `COMPILER_NAME ?=`), means it is also capable of taking input from CLI.



## Rodinia CUDA Compiler config Fix/Update logs

- Most of the changes has been done w.r.t `ivanradanov` [rodinia's](https://github.com/ivanradanov/rodinia/tree/cgo24) `cgo24` branch. Some are based on our config.

- Check [Docs/CUDA/CUDA_COMPILER_CONFIG_FIX_LOG.md](Docs/CUDA/CUDA_COMPILER_CONFIG_FIX_LOG.md) for details.


## Summary Flow

### Just Running

- **First time:** **Compile (i.e. `make CUDA`) > Prepare Datsets > Run `./scripts/run_timed_cuda.sh` script**
- **From Second time:** **Clean old cubins (i.e. `make CUDA_clean`) > Compile again (i.e. `make CUDA`) > Run `./scripts/run_timed_cuda.sh` script**



## How to compile + clean

- Set the `cuda-{VERSION}` (e.g. `cuda-11.4`) in [common/make.config](common/make.config)

- `make CUDA` will create `bin/linux/cuda/` (i.e. `bin/linux/{target_name}`) dir and dump cubins by algo folder names (e.g. `bin/linux/cuda/backprop`). You can also find the cubins in correspondent `cuda/{algo_name}` dir (e.g. `backprop` in `cuda/backprop/backprop`).

```sh
# Compile CUDA target (By default COMPILER_NAME=nvcc flag activated)
# By default, It will load the "nvcc.host.make.config"
make CUDA


# You can also pass "COMPILER_NAME=nvcc|polygeist" flag
# For "COMPILER_NAME=polygeist", it will load "polygeist.host.make.config"
make CUDA COMPILER_NAME=nvcc


# Compile with version specific "CUDA_INSTALL_PATH=" or, "CUDA_SAMPLES_PATH="
make CUDA CUDA_INSTALL_PATH=/usr/local/cuda-12.2 CUDA_SAMPLES_PATH=$HOME/Downloads/cuda-samples


# Compile with GPU arch targeted optimizations (defined in common/nvcc.host.make.config)
make CUDA GPU_TARGETED_ARCH_FLAGS="-gencode arch=compute_86,code=sm_86"


# DONOT USE "TYPE=CUDA" to compile for "nvcc".
# And also the "TYPE=" variable will be reserved for other compilers (i.e. polygeist)
# It will show unexpected behavior
make TYPE=CUDA COMPILER_NAME=nvcc


# ====== Cleaning ======


# Clean only CUDA target
make CUDA_clean


# Clean CUDA, OMP, OPENCL (Not recommended)
make clean
```


## How to prepare datasets

- **External data is only needed for `bfs`, `cfd`, `gaussian`, `hotspot`, `hotspot3D`, `lud`, `nn`, and `srad` algos. Rest of them will be auto generated**

- **`backprop`, `lavaMD`, `nw`, `streamcluster`, `particlefinder` and `pathfinder` needs no external data.**

- **Jump to [Docs/CUDA/PREPARE_CUDA_INPUT_DATASETS.md](Docs/CUDA/PREPARE_CUDA_INPUT_DATASETS.md) doc for the details of preparing datasets.**



## How to run the benchmark

- Run scripts have been collected from git repo [`ivanradanov/rodinia/`](https://github.com/ivanradanov/rodinia/tree/main/scripts) and little-bit updated.

- [`./scripts/run_timed_cuda.sh`](scripts/run_timed_cuda.sh) will run the benchmarks and dump outputs in `results/cuda/log` and **timing information** in `results/cuda/out`. This script will generate the correspondent output file, named with a timestamp (e.g. `results/cuda/out/2024-09-13T16:07:36,142487255+02:00.log`). Remember, After each run, it will generate a new log file to both `results/cuda/out` & `results/cuda/log` dirs.

- How the script works? `./scripts/run_timed_cuda.sh` will load the cuda app list from [`scripts/rodinia_cuda_apps_list.sh`](scripts/rodinia_cuda_apps_list.sh), then run each one of them using [`scripts/run_timed_common.sh`](scripts/run_timed_common.sh).


- **How to run?**

```sh
# Run "scripts/run_timed_cuda.sh" from benchmark root dir
./scripts/run_timed_cuda.sh

# if --delete-old-result-dir flag passed, delete old "results/" folder and dump new result log
./scripts/run_timed_cuda.sh --delete-old-result-dir
```

- (Not Important) [`scripts/parse_result.sh`](scripts/parse_result.sh), [`scripts/run_cpu.sh`](scripts/run_cpu.sh), [`scripts/run_gpu.sh`](scripts/run_gpu.sh) & [`scripts/run_wrap.sh`](scripts/run_wrap.sh) are by default inherited from core `rodinia`. They are not used.



## How to verify that results are correct? (verification)

- **(Important) Prerequisites:** You need to collect + prepare the datasets. Go through [Docs/CUDA/PREPARE_CUDA_INPUT_DATASETS.md](Docs/CUDA/PREPARE_CUDA_INPUT_DATASETS.md) doc.

- **3 steps: Compile with `MY_VERIFICATION_DISABLE=0` flag > Dump the data > then verify the data.**

- **Inside every CUDA algo, you will find a `run_verify` shell (e.g. [`cuda/backprop/run_verify`](cuda/backprop/run_verify)). This shell will be used to both generating the data and verifying the data.**

- **Verification core methods & utilities are written as `C` MACRO at [`common/my_verification.h`](common/my_verification.h)**

### Summary flow (If you just need command flow)

```sh
# Compile with `MY_VERIFICATION_DISABLE=0` flag
# make CUDA COMPILER_NAME=<nvcc|polygeist> MY_VERIFICATION_DISABLE=0
make CUDA MY_VERIFICATION_DISABLE=0
# Or,
make CUDA COMPILER_NAME=nvcc MY_VERIFICATION_DISABLE=0


# Dump verification data at `benchmark-root/data/verification_data/` dir
./scripts/dump_cuda_verification_data.sh

# if --delete-old-verification-data flag passed, delete old verification data folder and dump new data
./scripts/dump_cuda_verification_data.sh --delete-old-verification-data



# Check/verify results
./scripts/check_cuda_correctness.sh

# Or, if you want to remove the old result dir
./scripts/check_cuda_correctness.sh --delete-old-result-dir
```


### Dump verification data (Details)

First, you have to generate the result dataset for comparison.

- **Compile with `MY_VERIFICATION_DISABLE=0`:** For that, **you need to compile CUDA codes with specific flag `MY_VERIFICATION_DISABLE=0`. Or you can statically set it in [`common/make.config`](common/make.config).** This ENV variable is checked inside [`common/my_verification.h`](common/my_verification.h) as `if (getenv("MY_VERIFICATION_DUMP")) { \`.

```sh
# Compile
make CUDA MY_VERIFICATION_DISABLE=0

# Or,
# make CUDA COMPILER_NAME=<nvcc|polygeist> MY_VERIFICATION_DISABLE=0
make CUDA COMPILER_NAME=nvcc MY_VERIFICATION_DISABLE=0
```

- **Run [`scripts/dump_cuda_verification_data.sh`](scripts/dump_cuda_verification_data.sh):** `./scripts/dump_cuda_verification_data.sh` will load the cuda app list from [`scripts/rodinia_cuda_apps_list.sh`](scripts/rodinia_cuda_apps_list.sh), then run each one of them using [`scripts/run_timed_common.sh`](scripts/run_timed_common.sh) to generate the datasets.

```sh
./scripts/dump_cuda_verification_data.sh

# if --delete-old-verification-data flag passed, delete old verification data folder and dump new data
./scripts/dump_cuda_verification_data.sh --delete-old-verification-data
```

- **Where to find the dataset?** [`scripts/dump_cuda_verification_data.sh`](scripts/dump_cuda_verification_data.sh) will automatically create `benchmark-root/data/verification_data/` dir, and dump the data there. To be sure, after running this script, check the `benchmark-root/data/verification_data/` dir, if they are properly generated. You will find a ENV variable is exported as `export MY_VERIFICATION_DUMP=1`. The datagen dir handler is written at [`common/my_verification.h`](common/my_verification.h) check this variable like `char *verification_dir = getenv("MY_VERIFICATION_DIR"); \`. Then dump the data to that dir.



### Verify the results (Details)

- [`scripts/check_cuda_correctness.sh`](scripts/check_cuda_correctness.sh) to conduct result verification test. You will find the output in `results/cuda/log/` dir.

```sh
# Run
./scripts/check_cuda_correctness.sh


# Or, if you want to remove the old result dir
./scripts/check_cuda_correctness.sh --delete-old-result-dir


# In returned log file
-------------- running bfs @ 2024-09-17T17:20:46,548274425+02:00 --------------
Starting verification of h_cost of type int from file /abs/path/to/benchmark-root/data/verification_data//bfs/bfs.cu:264
Verification of h_cost ended
result: PASS
largest absolute error: 0
largest relative error: 0
....
...
-------------- running nw @ 2024-09-17T17:21:02,535161857+02:00 --------------
Starting verification of output_itemsets of type int from file /abs/path/to/benchmark-root/data/verification_data//nw/needle.cu:200
Verification of output_itemsets ended
result: PASS
largest absolute error: 0
largest relative error: 0
....
...
```






## Kernels that are not used


### `Kmeans`, `leukocyte`, `mummergpu`, & `hybridsort`

- Because they use texture memory which is not supported by Polygeist.
- Texture reference support has been removed from CUDA 12.0 (Src- Polygeist paper (page 7), [CUDA 12.0 - Still support for texture reference?](https://forums.developer.nvidia.com/t/cuda-12-0-still-support-for-texture-reference-support-for-pascal-architecture-warp-synchronous-programming/237284))


### `huffman` and `heartwall`

Use unsupported features within Polygeist (virtual functions). (Src- [Polygeist GPU paper](https://c.wsmoses.com/papers/polygeist24.pdf) (page 7))


### `dwt2d`, `b+tree`, and `srad_v2`

Produce non-deterministic results in baseline and likely are buggy benchmarks. (Src- [Polygeist GPU paper](https://c.wsmoses.com/papers/polygeist24.pdf) (page 7))


## Special Note

### `streamcluster`

- Once found some weird behavior for Compute capability `7.5` (i.e. `-gencode arch=compute_75,code=sm_75`). But now it is working. But need to be careful.