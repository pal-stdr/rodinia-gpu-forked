# Collect the data sets

## Polygeist specific

- **External data is only needed for `bfs`, `cfd`, `gaussian`, `hotspot`, `hotspot3D`, `lud`, `nn`, and `srad` algos. Rest of them will be auto generated**

- **(Recommended) To run for polygeist regarding experiment, download [polygeist-cgo24-artifact-data.tar.gz](https://zenodo.org/records/10465934). Then `unzip` it. You should see a gigantic list like following. You need the contents of `polygeist-cgo24-artifact-data/data/` dir.**

```sh
polygeist-cgo24-artifact-data/
    ├── cuda-10.2-hip-samples
    │   ├── 0_Simple
    │   ├── 1_Utilities
    ....
    ....
    │   ├── 7_CUDALibraries
    │   ├── common
    │   ├── EULA.txt
    │   └── Makefile
    ├── cuda-10.2-samples
    │   ├── 0_Simple
    ....
    ....
    │   └── Makefile
    ├── data # <== You need this
    │   ├── bfs
    │   ├── b+tree
    │   ├── cfd
    │   ├── dwt2d
    │   ├── gaussian
    │   ├── heartwall
    │   ├── hotspot
    │   ├── hotspot3D
    │   ├── huffman
    │   ├── hybridsort
    │   ├── kmeans
    │   ├── leukocyte
    │   ├── lud
    │   ├── mummergpu
    │   ├── myocyte
    │   ├── nn
    │   └── srad
    ├── out.txt
    ├── Polygeist
    │   ├── AddClang.cmake
    ....
    ....
    │   └── tools
    └── rodinia
        ├── common
        ├── cuda
        ├── data
    ....
    ....
        └── scripts
```
- **Copy `bfs`, `cfd`, `gaussian`, `hotspot`, `hotspot3D`, `lud`, `nn`, `srad` dirs from `polygeist-cgo24-artifact-data/data/` dir to `rodinia-gpu-forked/data/` dir so that the contents are arranged like following structure**

```sh
rodinia-gpu-forked/data/
    ├── bfs
    │   ├── graph1MW_6.txt
    │   ├── graph4096.txt
    │   ├── graph65536.txt
    │   └── inputGen
    │       ├── gen_dataset.sh
    │       ├── graphgen.cpp
    │       └── Makefile
    ├── cfd
    │   ├── fvcorr.domn.097K
    │   ├── fvcorr.domn.193K
    │   └── missile.domn.0.2M
    ├── gaussian
    │   ├── matrix1024.txt
    │   ├── matrix16.txt
    │   ├── matrix208.txt
    │   ├── matrix3.txt
    │   ├── matrix4.txt
    │   └── matrixGenerator.py
    ├── hotspot
    │   ├── inputGen
    │   │   ├── 1024_16384.h
    │   │   ├── 1024_2048.h
    │   │   ├── 1024_4096.h
    │   │   ├── 1024_8192.h
    │   │   ├── 64_128.h
    │   │   ├── 64_256.h
    │   │   ├── hotspotex.cpp
    │   │   ├── hotspotver.cpp
    │   │   ├── Makefile
    │   │   └── README
    │   ├── power_1024
    │   ├── power_2048
    │   ├── power_4096
    │   ├── power_512
    │   ├── power_64
    │   ├── temp_1024
    │   ├── temp_2048
    │   ├── temp_4096
    │   ├── temp_512
    │   └── temp_64
    ├── hotspot3D
    │   ├── power_512x2
    │   ├── power_512x4
    │   ├── power_512x8
    │   ├── power_64x8
    │   ├── temp_512x2
    │   ├── temp_512x4
    │   ├── temp_512x8
    │   └── temp_64x8
    ├── lud
    │   ├── 2048.dat
    │   ├── 256.dat
    │   ├── 512.dat
    │   └── 64.dat
    ├── nn
    │   ├── cane4_0.db
    │   ├── cane4_1.db
    │   ├── cane4_2.db
    │   ├── cane4_3.db
    │   ├── filelist.txt
    │   └── inputGen
    │       ├── gen_dataset_multifile.sh
    │       ├── gen_dataset.sh
    │       ├── hurricanegen.c
    │       └── Makefile
    └── srad
        └── image.pgm
```

- **`backprop`, `lavaMD`, `nw`, `streamcluster`, `particlefinder` and `pathfinder` needs no external data.**




## For General experiments

- **External data is only needed for `bfs`, `cfd`, `gaussian`, `hotspot`, `hotspot3D`, `lud`, `nn`, and `srad` algos. Rest of them will be auto generated**

- **To run general benchmarking experiment, download [rodinia_3.1.tar.bz2](http://lava.cs.virginia.edu/Rodinia/download_links.htm). Then `unzip` it. You should see a gigantic list like following. You need the contents of `rodinia_3.1/data/` dir.**

```sh
├── bin
├── common
├── cuda
│   ├── backprop
...
....
│   └── streamcluster
├── data # <== You need this
│   ├── bfs
│   ├── b+tree
│   ├── cfd
│   ├── dwt2d
│   ├── gaussian
│   ├── heartwall
│   ├── hotspot
│   ├── hotspot3D
│   ├── huffman
│   ├── hybridsort
│   ├── kmeans
│   ├── leukocyte
│   ├── lud
│   ├── mummergpu
│   ├── myocyte
│   ├── nn
│   └── srad
├── LICENSE
├── Makefile
├── opencl
│   ├── backprop
│   ├── bfs
...
....
│   └── streamcluster
├── openmp
│   ├── backprop
...
....
│   └── streamcluster
├── others
│   └── rng
└── README
```

- **Copy `bfs`, `cfd`, `gaussian`, `hotspot`, `hotspot3D`, `lud`, `nn`, `srad` dirs from `rodinia_3.1/data/` dir to `rodinia-gpu-forked/data/` dir so that the contents are arranged like following structure**

```sh
rodinia-gpu-forked/data/
    ├── bfs
    │   ├── graph1MW_6.txt
    │   ├── graph4096.txt
    │   ├── graph65536.txt
    │   └── inputGen
    │       ├── gen_dataset.sh
    │       ├── graphgen.cpp
    │       └── Makefile
    ├── cfd
    │   ├── fvcorr.domn.097K
    │   ├── fvcorr.domn.193K
    │   └── missile.domn.0.2M
    ├── gaussian
    │   ├── matrix1024.txt
    │   ├── matrix16.txt
    │   ├── matrix208.txt
    │   ├── matrix3.txt
    │   ├── matrix4.txt
    │   └── matrixGenerator.py
    ├── hotspot
    │   ├── inputGen
    │   │   ├── 1024_16384.h
    │   │   ├── 1024_2048.h
    │   │   ├── 1024_4096.h
    │   │   ├── 1024_8192.h
    │   │   ├── 64_128.h
    │   │   ├── 64_256.h
    │   │   ├── hotspotex.cpp
    │   │   ├── hotspotver.cpp
    │   │   ├── Makefile
    │   │   └── README
    │   ├── power_1024
    │   ├── power_2048
    │   ├── power_4096
    │   ├── power_512
    │   ├── power_64
    │   ├── temp_1024
    │   ├── temp_2048
    │   ├── temp_4096
    │   ├── temp_512
    │   └── temp_64
    ├── hotspot3D
    │   ├── power_512x2
    │   ├── power_512x4
    │   ├── power_512x8
    │   ├── power_64x8
    │   ├── temp_512x2
    │   ├── temp_512x4
    │   ├── temp_512x8
    │   └── temp_64x8
    ├── lud
    │   ├── 2048.dat
    │   ├── 256.dat
    │   ├── 512.dat
    │   └── 64.dat
    ├── nn
    │   ├── cane4_0.db
    │   ├── cane4_1.db
    │   ├── cane4_2.db
    │   ├── cane4_3.db
    │   ├── filelist.txt
    │   └── inputGen
    │       ├── gen_dataset_multifile.sh
    │       ├── gen_dataset.sh
    │       ├── hurricanegen.c
    │       └── Makefile
    └── srad
        └── image.pgm
```

