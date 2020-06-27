export PREFIX="$HOME/opt/cross/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"

SYSROOT="../sysroot"

wget https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.xz
tar xf binutils-2.34.tar.xz
patch -p0 < binutils.patch

cd binutils-2.34

mkdir build && cd build
../configure -target=$TARGET --prefix=$PREFIX --disable-nls --with-sysroot=$SYSROOT --disable-werror
make
make install


cd ../..
rm -rf binutils-2.24
rm binutils-2.34.tar.xz
