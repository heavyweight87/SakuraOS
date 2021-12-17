#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

export PREFIX="/opt/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"
cd ${DIR} && make
cp ${DIR}/program.o ${DIR}/../build/bin/isodir/boot/
