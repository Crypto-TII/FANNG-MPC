#!/bin/bash

mpclib_installation_directory=$(pwd)

echo "===== Preparing FANNG-MPC Framework ====="
echo ""
echo "This script helps you set up the requirements for using the FANNG-MPC framework."
echo ""

echo "------ Stage 1: Installing Necessary Dependencies ------"
sudo apt-get install -y yasm
sudo apt-get install -y curl
sudo apt-get install -y wget
sudo apt-get install -y libboost-dev #Tested with version 1.71.0.0
sudo apt-get install -y libmysqlcppconn-dev #Tested with version 1.1.9-1
sudo apt-get install -y m4
sudo apt-get install -y g++
sudo apt-get install -y make
sudo apt-get install -y unzip
sudo DEBIAN_FRONTEND=noninteractive apt-get -y install cmake


#install python dependencies
sudo apt-get install -y python
sudo curl -O 'https://bootstrap.pypa.io/pip/2.7/get-pip.py'
python2 get-pip.py
python2 -m pip install numpy
rm -f get-pip.py


#install RustC
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source $HOME/.cargo/env
rustup default nightly
rustup target add wasm32-unknown-unknown
source rustup

mylocal="/opt"
cd $mylocal 

#install MPIR
sudo curl -O 'https://src.fedoraproject.org/lookaside/extras/mpir/mpir-3.0.0.tar.bz2/sha512/c735105db8b86db739fd915bf16064e6bc82d0565ad8858059e4e93f62c9d72d9a1c02a5ca9859b184346a8dc64fa714d4d61404cff1e405dc548cbd54d0a88e/mpir-3.0.0.tar.bz2'
sudo tar xf mpir-3.0.0.tar.bz2
cd mpir-3.0.0 
sudo ./configure --enable-cxx --prefix="${mylocal}/mpir"
sudo make && sudo make check && sudo make install
sudo rm $mylocal/mpir-3.0.0.tar.bz2

#install OpenSSL 1.1.1g
cd $mylocal 
sudo wget 'https://www.openssl.org/source/openssl-1.1.1g.tar.gz'
sudo tar -xf openssl-1.1.1g.tar.gz
cd openssl-1.1.1g 
sudo ./config --prefix="${mylocal}/openssl"
sudo make && sudo make install
sudo rm $mylocal/openssl-1.1.1g.tar.gz

# install crypto++
cd $mylocal 
sudo wget https://github.com/weidai11/cryptopp/releases/download/CRYPTOPP_8_2_0/cryptopp820.zip
sudo unzip cryptopp820.zip -d cryptopp820
cd cryptopp820 
sudo make && sudo make install PREFIX=${mylocal}/cryptopp
sudo rm $mylocal/cryptopp820.zip

#install catch2
cd $mylocal 
sudo wget https://github.com/catchorg/Catch2/archive/refs/heads/v2.x.zip --no-check-certificate
sudo unzip v2.x.zip
cd Catch2-2.x 
sudo cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
sudo rm $mylocal/v2.x.zip

#install cereal (serialization library)
sudo apt install libcereal-dev


echo "------ Stage 2: Setting up environment variables ------"
# export env variables to bashrc
cd $mpclib_installation_directory/installation 
cat export-variables.txt >> ~/.bashrc
source ~/.bashrc

echo "------ Stage 3: Setting up the Configuation ------"
cd $mpclib_installation_directory
cp CONFIG CONFIG.mine
sed -i "s|ROOT = /users/<user>/<repo>|ROOT = $mpclib_installation_directory|" "CONFIG.mine"
sed -i "s|OSSL = /opt|OSSL = /opt/ossl|" "CONFIG.mine"


echo "------ Stage 4: Compiling the project ------"
make progs


echo "------ Stage 5: Selecting the protocol variant ------"
read -p "Enter the protocol variant [Default: 1 (Shamir)]: " prot_variant
if [[ ! $prot_variant =~ ^[0-9]+$ ]] || ((prot_variant < 1 || prot_variant > 27)); then
  echo "Invalid input. The protocol variant must be a number between 1 and 27. Defaulting to 1."
  prot_variant=1
fi
cp Auto-Test-Data/$prot_variant/* Data/
cp Auto-Test-Data/Cert-Store/* Cert-Store/


echo "------ FANNG-MPC IS NOW READY TO USE ------"