# conda-package-demo (LINUX ONLY)

This project demonstrates how to build and distribute a compiled C application as a Conda package from scratch. It includes a simple CLI tool (zhello) and a reusable C library (libzhello) that perform data compression using the widely used zlib library.

Anaconda.org package:
https://anaconda.org/pieczka.patryk/zhello

## Project layout
```text
conda-package-demo/
├── Dockerfile
├── LICENSE
├── README.md
├── run-all.sh
├── .gitignore
├── src/
│   ├── Makefile
│   ├── libzhello.c
│   ├── libzhello.h
│   └── zhello.c
├── tests/
│   └── sample.txt
└── recipe/
    ├── meta.yaml
    ├── build.sh
    └── bld.bat
```

## Build Docker image
```bash
docker build -t conda-package-demo .
```

## Open a shell inside the Docker container
```bash
docker run --rm -it -v "$PWD":/workspace conda-package-demo bash -c "cd /workspace && exec bash"
```

## Build the Conda recipe
```bash
conda build recipe
```

## Install the locally built package
```bash
conda install --use-local zhello -y
```

## CLI examples
```bash
zhello "Hello Conda"
zhello --level 9 --repeat 1000 "compress me"
zhello --file tests/sample.txt
zhello --json "structured output"
zhello --save-compressed out.bin "persist compressed payload"
```

## Upload to Anaconda.org

```bash
anaconda -t TOKEN upload /opt/conda/conda-bld/linux-64/zhello-*.conda
```
## Install from Anaconda.org

Install the package from Anaconda.org:

```bash
conda install -c pieczka.patryk zhello
```

