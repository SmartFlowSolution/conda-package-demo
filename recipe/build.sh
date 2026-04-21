#!/usr/bin/env bash
set -euxo pipefail

cd "${SRC_DIR}/src"

make clean
make \
  CC="${CC}" \
  AR="${AR}" \
  CPPFLAGS="${CPPFLAGS} -I${PREFIX}/include" \
  CFLAGS="${CFLAGS}" \
  LDFLAGS="${LDFLAGS} -L${PREFIX}/lib"
make install PREFIX="${PREFIX}"
