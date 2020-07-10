DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SYSROOT=${DIR}/../../sysroot

export PREFIX="$HOME/opt/cross/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"

../configure --prefix=/usr --target=i686-sakura
make all -j4
make DESTDIR=${SYSROOT} install

echo "Copying to rootfs"
cp -ar $SYSROOT/usr/i686-sakura/* $SYSROOT/usr/
rm -rf $SYSROOT/usr/i686-sakura/
echo "Done"

