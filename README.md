# Rodinia Benchmark for CUDA + Polygeist-GPU

- (The original rodinia readme is at [README](README))

- Benchmark in the light of `ivanradanov's` work [rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24), based on the paper [Retargeting and Respecializing GPU Workloads for Performance Portability](https://c.wsmoses.com/papers/polygeist24.pdf). The repo link is at the `cgo24` branch of [Polygeist](https://github.com/llvm/Polygeist/tree/cgo24)




# Objectives

- Developing a benchmark for rodinia CUDA and Polygeist-GPU targeting CUDA.




# Rodinia Compiler config Fix/Update logs

- Most of the changes has been done w.r.t `ivanradanov` [rodinia's](https://github.com/ivanradanov/rodinia/tree/cgo24) `cgo24` branch. Some are based on our config.

- Check [COMPILER_CONFIG_FIX_LOG.md](COMPILER_CONFIG_FIX_LOG.md) for details.



# Pre-requisites

- `CUDA-11.4` installed (because [rodinia](https://github.com/ivanradanov/rodinia/tree/cgo24) benchmark was done in `CUDA-11.4`)

- For CUDA specific compilation flag knowledge, jump to [Docs/CUDA/TARGETING_GPU_ARCH_with_COMPILATION_FLAGS.md](Docs/CUDA/TARGETING_GPU_ARCH_with_COMPILATION_FLAGS.md).



# How to use

## How to compile + clean

- `make CUDA` will create `bin/linux/cuda/` (i.e. `bin/linux/{target_name}`) dir and dump cubins by algo folder names (e.g. `bin/linux/cuda/backprop`).

```sh
# Compile CUDA target
make CUDA

# Clean only CUDA target
make CUDA_clean

# Clean CUDA, OMP, OPENCL (Not recommended)
make clean
```


# Kernels that are not used

## `Kmeans`, `leukocyte`, `mummergpu`, & `hybridsort`

- Because they use texture memory which is not supported by Polygeist.
- Texture reference support has been removed from CUDA 12.0 (Src- Polygeist paper (page 7), [CUDA 12.0 - Still support for texture reference?](https://forums.developer.nvidia.com/t/cuda-12-0-still-support-for-texture-reference-support-for-pascal-architecture-warp-synchronous-programming/237284))


## `huffman` and `heartwall`

Use unsupported features within Polygeist (virtual functions). (Src- [Polygeist GPU paper](https://c.wsmoses.com/papers/polygeist24.pdf) (page 7))


## `dwt2d`, `b+tree`, and `srad_v2`

Produce non-deterministic results in baseline and likely are buggy benchmarks. (Src- [Polygeist GPU paper](https://c.wsmoses.com/papers/polygeist24.pdf) (page 7))


