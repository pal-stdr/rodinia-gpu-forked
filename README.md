# Rodinia Benchmark for CUDA + Polygeist-GPU

- (The original rodinia readme is at [README](README))

- Benchmark in the light of `ivanradanov's` work [rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24), based on the paper [Retargeting and Respecializing GPU Workloads for Performance Portability](https://c.wsmoses.com/papers/polygeist24.pdf). The repo link is at the `cgo24` branch of [Polygeist](https://github.com/llvm/Polygeist/tree/cgo24)


# Acknowledgement

- Lot of code has been collected from [rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24). So atmost gratitude for them.


# Objectives

- Developing a benchmark for rodinia CUDA and Polygeist-GPU targeting CUDA.




# Rodinia Compiler config Fix/Update logs

- Most of the changes has been done w.r.t `ivanradanov` [rodinia's](https://github.com/ivanradanov/rodinia/tree/cgo24) `cgo24` branch. Some are based on our config.

- Check [COMPILER_CONFIG_FIX_LOG.md](COMPILER_CONFIG_FIX_LOG.md) for details.



# Pre-requisites

- For this repo, `CUDA-11.4` is tested & used.

- `CUDA-11.4` needed if you want to work with [ivanradanov's rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24). Because that benchmark was done with `CUDA-11.4`.

- For general experiments, you should be able to use `CUDA-11.X` or `CUDA-12.X`.

- For CUDA specific compilation flag knowledge, jump to [Docs/CUDA/TARGETING_GPU_ARCH_with_COMPILATION_FLAGS.md](Docs/CUDA/TARGETING_GPU_ARCH_with_COMPILATION_FLAGS.md).



# How to use `nvcc` (i.e. just CUDA) compilation flow?

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


# Clean only CUDA target
make CUDA_clean


# Clean CUDA, OMP, OPENCL (Not recommended)
make clean
```


## How to prepare datasets

- **External data is only needed for `bfs`, `cfd`, `gaussian`, `hotspot`, `hotspot3D`, `lud`, `nn`, and `srad` algos. Rest of them will be auto generated**

- **`backprop`, `lavaMD`, `nw`, `streamcluster`, `particlefinder` and `pathfinder` needs no external data.**

- **Jump to [PREPARE_CUDA_INPUT_DATASETS.md](PREPARE_CUDA_INPUT_DATASETS.md) doc for the details of preparing datasets.**



## How to run the benchmark

- Run scripts have been collected from git repo [`ivanradanov/rodinia/`](https://github.com/ivanradanov/rodinia/tree/main/scripts) and little-bit updated.

- [`./scripts/run_timed_cuda.sh`](scripts/run_timed_cuda.sh) will run the benchmarks and dump outputs in `results/cuda/log` and **timing information** in `results/cuda/out`. This script will generate the correspondent output file, named with a timestamp (e.g. `results/cuda/out/2024-09-13T16:07:36,142487255+02:00.log`). Remember, After each run, it will generate a new log file to both `results/cuda/out` & ``results/cuda/log` dirs.

- How the script works? `./scripts/run_timed_cuda.sh` will load the cuda app list from [`scripts/rodinia_cuda_apps_list.sh`](scripts/rodinia_cuda_apps_list.sh), then run each one of them using [`scripts/run_timed_common.sh`](scripts/run_timed_common.sh).


- **How to run?**

```sh
# Run "scripts/run_timed_cuda.sh" from benchmark root dir
./scripts/run_timed_cuda.sh
```

- (Not Important) [`scripts/parse_result.sh`](scripts/parse_result.sh), [`scripts/run_cpu.sh`](scripts/run_cpu.sh), [`scripts/run_gpu.sh`](scripts/run_gpu.sh) & [`scripts/run_wrap.sh`](scripts/run_wrap.sh) are by default inherited from core `rodinia`. They are not used.



# Kernels that are not used

## `Kmeans`, `leukocyte`, `mummergpu`, & `hybridsort`

- Because they use texture memory which is not supported by Polygeist.
- Texture reference support has been removed from CUDA 12.0 (Src- Polygeist paper (page 7), [CUDA 12.0 - Still support for texture reference?](https://forums.developer.nvidia.com/t/cuda-12-0-still-support-for-texture-reference-support-for-pascal-architecture-warp-synchronous-programming/237284))


## `huffman` and `heartwall`

Use unsupported features within Polygeist (virtual functions). (Src- [Polygeist GPU paper](https://c.wsmoses.com/papers/polygeist24.pdf) (page 7))


## `dwt2d`, `b+tree`, and `srad_v2`

Produce non-deterministic results in baseline and likely are buggy benchmarks. (Src- [Polygeist GPU paper](https://c.wsmoses.com/papers/polygeist24.pdf) (page 7))


