FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-lc"]

RUN apt-get update && apt-get install -y --no-install-recommends \
    wget git ca-certificates bzip2 build-essential file \
    && rm -rf /var/lib/apt/lists/*

RUN wget -O /tmp/miniforge.sh \
    https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh && \
    bash /tmp/miniforge.sh -b -p /opt/conda && \
    rm /tmp/miniforge.sh

ENV PATH=/opt/conda/bin:$PATH

RUN conda config --set channel_priority strict && \
    conda update -n base -c conda-forge conda && \
    conda install -n base -c conda-forge \
        conda-build \
        anaconda-client && \
    conda clean -afy