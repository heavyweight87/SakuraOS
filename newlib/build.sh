DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SYSROOT=${DIR}/../sysroot

cecho(){
    RED="\033[0;31m"
    GREEN="\033[0;32m"
    YELLOW="\033[1;33m"
    NC="\033[0m" # colour off

    printf "${!1}${2} ${NC}\n"
}

export PREFIX="/opt/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"

mkdir -p build && cd build

wget -N https://ftp.gnu.org/gnu/automake/automake-1.11.tar.gz
wget -N https://ftp.gnu.org/gnu/autoconf/autoconf-2.65.tar.gz
tar xf automake-1.11.tar.gz
tar xf autoconf-2.65.tar.gz
mkdir -p auto-build && cd auto-build

# build automake
../automake-1.11/configure --prefix="${DIR}/build/auto-build"
make && make install

# build autoconf
../autoconf-2.65/configure --prefix="${DIR}/build/auto-build/"
make && make install

export PATH=${DIR}/build/auto-build/bin:$PATH

cd ..
../configure --prefix=/usr --target=i686-sakura
make all -j8

cecho "GREEN" "Installing..."
make DESTDIR=${SYSROOT} install 

cecho "GREEN" "Copying to $SYSROOT"
cp -ar $SYSROOT/usr/i686-sakura/* $SYSROOT/usr/
rm -rf $SYSROOT/usr/i686-sakura/
cecho "GREEN" "Done"

rm -rf ${DIR}/build

