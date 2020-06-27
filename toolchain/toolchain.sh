export PREFIX="$HOME/opt/cross/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"

SYSROOT=="$(dirname "$(pwd)")"/sysroot

wget https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.xz
tar xf binutils-2.34.tar.xz
patch -p0 < binutils.patch

cd binutils-2.34

mkdir build && cd build
../configure -target=$TARGET --prefix=$PREFIX --disable-nls --with-sysroot=$SYSROOT --disable-werror
make -j$(nproc)
make install
cd ../..

# install GCC

wget ftp://ftp.gnu.org/gnu/gcc/gcc-10.1.0/gcc-10.1.0.tar.gz
tar xf gcc-10.1.0.tar.gz
patch -p0 < gcc.patch

cd gcc-10.1.0
mkdir build && cd build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --with-sysroot=$SYSROOT --enable-libstdcxx --with-newlib
echo $SYSROOT
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
make all-target-libstdc++-v3 -j$(nproc)
make install-target-libstdc++-v3

cd ../..

rm -rf binutils-2.34
rm binutils-2.34.tar.xz
rm -rf gcc-10.1.0
rm gcc-10.1.0.tar.gz
