# Rodinia CUDA Algo Fix/Update logs

- Most of the changes has been done collected from `ivanradanov [rodinia's cuda dir](https://github.com/ivanradanov/rodinia/tree/cgo24/cuda) `cgo24` branch.


# Pre-requisite

- **Collect [cgo24/common/my_timing.h](https://github.com/ivanradanov/rodinia/tree/cgo24/common/), [cgo24/common/my_verification.h](https://github.com/ivanradanov/rodinia/tree/cgo24/common/my_verification.h) and add here  [`my_timing.h`](common/my_timing.h) & [`my_verification.h`](common/my_verification.h). They are for measuring time and result verification.**

- **[`common/make.config`](common/make.config) update to add `my_timing.h`, `my_verification.h` in `-I` include path.**


# 1. Backprop [cuda/backprop/](cuda/backprop/) update

- Updated `backprop.c`, `backprop.h`, `imagenet.c`, `backprop_cuda.cu`, `backprop_cuda_kernel.cu`,  `facetrain.c`

- add `dal.c`

- Run config update `run`



# 2. bfs [cuda/bfs/](cuda/bfs/) update

- Only change in `bfs.cu`

- Run config update `run`



# 3. cfd [cuda/cfd/](cuda/cfd/) update

- Only change in `euler3d.cu`

- [`common/make.config`](common/make.config) adjusted getting headers `helper_cuda.h`, `helper_timer.h` from `/usr/local/$(CUDA_DIR)/samples/common/inc/` path

```Makefile
SDK_DIR = /usr/local/$(CUDA_DIR)/samples/
CUDA_SAMPLES_PATH_ = $(SDK_DIR)
```

- Run config update `run`



# 4. gaussian [cuda/gaussian/](cuda/gaussian/) update

- Only change in `gaussian.cu`

- Run config update `run`



# 5. hotspot [cuda/hotspot/](cuda/hotspot/) update

- Only change in `hotspot.cu`

- Run config update `run`



# 6. hotspot3D [cuda/hotspot3D/](cuda/hotspot3D/) update

- Only change in `3D.cu`

- Run config update `run`



# 7. lavaMD [cuda/lavaMD/](cuda/lavaMD/) update

- Changes in `cuda/lavaMD/kernel/kernel_gpu_cuda_wrapper.cu`, `cuda/lavaMD/util/device/device.cu`, `cuda/lavaMD/timer/timer.c`, `cuda/lavaMD/main.cu`, `cuda/lavaMD/main.h`

- Run config update `run`



# 8. lud [cuda/lud/](cuda/lud/) update

- Changes in `cuda/lud/base/Makefile`, `cuda/lud/common/common.c`, `cuda/lud/cuda/lud_kernel.cu`, `cuda/lud/cuda/lud.cu`, `cuda/lud/main.h`

- Run config update `run`



# 9. nn [cuda/nn/](cuda/nn/) update

- Only change in `nn_cuda.cu`

- Run config update `run`



# 10. nw [cuda/nw/](cuda/nw/) update

- Only change in `needle.cu`

- Run config update `run`



# 11. streamcluster [cuda/streamcluster/](cuda/streamcluster/) update

- Changes in `cuda/streamcluster/streamcluster_cuda_cpu.cpp`, `cuda/streamcluster/streamcluster_cuda.cu`, `cuda/streamcluster/streamcluster_header.cu`

- Run config update `run`



# 12. particlefilter [cuda/particlefilter/](cuda/particlefilter/) update

- Changes in `cuda/particlefilter/ex_particle_CUDA_float_seq.cu`, `cuda/particlefilter/ex_particle_CUDA_naive_seq.cu`

- Run config update `run`