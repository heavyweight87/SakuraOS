DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SYSROOT=${DIR}/../../sysroot

../configure --prefix=/usr --target=i686-sakura
make all -j4
make DESTDIR=${SYSROOT} install

cp -ar $SYSROOT/usr/i686-sakura/* $SYSROOT/usr/
rm -rf $SYSROOT/usr/i686-sakura/

