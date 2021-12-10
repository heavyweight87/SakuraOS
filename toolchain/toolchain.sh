export PREFIX="/opt/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"

SYSROOT="$(dirname "$(pwd)")/sysroot"

if [ $# -eq 0 ] ; then
    run_gcc=true
    run_binutils=true
fi

if [[ $1 == '-g' ]] ; then
    run_gcc=true
    run_binutils=false
fi

if [[ $1 == '-b' ]] ; then
    run_gcc=false
    run_binutils=true
fi

if [ "$run_binutils" == true ]
then

wget https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.xz
tar xf binutils-2.34.tar.xz
patch -p0 < binutils.patch

cd binutils-2.34

mkdir build && cd build
../configure -target=$TARGET --prefix=$PREFIX --disable-nls --with-sysroot=$SYSROOT --disable-werror
make -j$(nproc)
make install
cd ../..

rm -rf binutils-2.34
rm binutils-2.34.tar.xz

fi

if [ "$run_gcc" == true ]
then

# install GCC

wget ftp://ftp.gnu.org/gnu/gcc/gcc-10.1.0/gcc-10.1.0.tar.gz
tar xf gcc-10.1.0.tar.gz
patch -p0 < gcc.patch

cd gcc-10.1.0
mkdir build && cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --with-sysroot=$SYSROOT --enable-libstdcxx --with-newlib
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
make all-target-libstdc++-v3 -j$(nproc)
make install-target-libstdc++-v3

cd ../..

rm -rf gcc-10.1.0
rm gcc-10.1.0.tar.gz

fi
