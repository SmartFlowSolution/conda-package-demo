#!/usr/bin/env bash
set -euxo pipefail

docker build -t conda-package-demo .
docker run --rm -it -v "$PWD":/workspace conda-package-demo bash -lc '
  cd /workspace
  conda build recipe
  conda install --use-local zhello -y
  zhello --json "Hello"
  zhello --file tests/sample.txt
'
