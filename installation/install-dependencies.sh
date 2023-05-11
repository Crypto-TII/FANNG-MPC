#!/bin/bash

mpclib_installation_directory=$(pwd)

#Prerequirements
sudo apt-get install -y yasm
sudo apt-get install -y curl
sudo apt-get install -y libboost-dev #Tested with version 1.71.0.0
sudo apt-get install -y libmysqlcppconn-dev #Tested with version 1.1.9-1
sudo apt-get install -y m4
sudo apt-get install -y g++
sudo apt-get install -y make
sudo apt-get install -y unzip
sudo apt-get -y install cmake
#install python dependencies
sudo apt-get install -y python
sudo curl -O 'https://bootstrap.pypa.io/pip/2.7/get-pip.py'
python2 get-pip.py
python2 -m pip install numpy
rm -f get-pip.py

#install RustC
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source $HOME/.cargo/env
rustup default nightly
rustup target add wasm32-unknown-unknown
source rustup

mylocal="/opt"
cd $mylocal || exit

#install MPIR
sudo curl -O 'https://src.fedoraproject.org/lookaside/extras/mpir/mpir-3.0.0.tar.bz2/sha512/c735105db8b86db739fd915bf16064e6bc82d0565ad8858059e4e93f62c9d72d9a1c02a5ca9859b184346a8dc64fa714d4d61404cff1e405dc548cbd54d0a88e/mpir-3.0.0.tar.bz2'
sudo tar xf mpir-3.0.0.tar.bz2
cd mpir-3.0.0 || exit
sudo ./configure --enable-cxx --prefix="${mylocal}/mpir"
sudo make && sudo make check && sudo make install

#install OpenSSL 1.1.1g
cd $mylocal || exit
sudo curl -O 'https://www.openssl.org/source/openssl-1.1.1g.tar.gz'
sudo tar -xf openssl-1.1.1g.tar.gz
cd openssl-1.1.1g || exit
sudo ./config --prefix="${mylocal}/openssl"
sudo make && sudo make install

# install crypto++
cd $mylocal || exit
sudo curl -O https://www.cryptopp.com/cryptopp820.zip
sudo unzip cryptopp820.zip -d cryptopp820
cd cryptopp820 || exit
sudo make && sudo make install PREFIX=${mylocal}/cryptopp

#install catch2
cd $mylocal || exit
sudo wget https://github.com/catchorg/Catch2/archive/refs/heads/v2.x.zip --no-check-certificate
sudo unzip v2.x.zip
cd Catch2-2.x || exit
sudo cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install

#install cereal (serialization library)
sudo apt install libcereal-dev

# export env variables to bashrc
cd $mpclib_installation_directory/installation || exit
cat export-variables.txt >> ~/.bashrc
echo "------ ALL DONE SUCCESSFULLY ------"
