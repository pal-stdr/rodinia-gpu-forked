# Rodinia CUDA Algo Fix/Update logs

- Most of the changes has been done collected from `ivanradanov [rodinia's cuda dir](https://github.com/ivanradanov/rodinia/tree/cgo24/cuda) `cgo24` branch.


# Pre-requisite

- **Collect [cgo24/common/my_timing.h](https://github.com/ivanradanov/rodinia/tree/cgo24/common/), [cgo24/common/my_verification.h](https://github.com/ivanradanov/rodinia/tree/cgo24/common/my_verification.h) and add here  [`my_timing.h`](common/my_timing.h) & [`my_verification.h`](common/my_verification.h). They are for measuring time and result verification.**

- **[`common/make.config`](common/make.config) update to add `my_timing.h`, `my_verification.h` in `-I` include path.**


# 1. Backprop [cuda/backprop/](cuda/backprop/) update

- Updated `backprop.c`, `backprop.h`, `imagenet.c`, `backprop_cuda.cu`, `backprop_cuda_kernel.cu`,  `facetrain.c`

- add `dal.c`

- Run config update `run`
