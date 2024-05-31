# MPCLib installation guide

## Quick install
Before setting up the project, make sure your CPU supports [these instructions](#cpu-support).

Run, from the root folder of the repo `source installation/install-depedencies.sh` to quickly install and setup the project dependencies and required libraries.

[Continue to step 3](#step-3)

__NOTE:__ The script is not infallible, if it breaks at some point it might be because a link is broken. We recommend you to try manually the content of the script, from the point it broke, 1 instruction at the time.
## Prerequisites

Make sure you meet the following requirements before proceeding with the installation:

<a id="cpu-support"></a>
### CPU supporting AES-NI and PCLMUL

**If one of these instructions is not supported by your CPU, Scale/Mamba will not run as expected**. Check it following these steps:

- Install _cpuid_. This tool provides CPU related information such as the instructions supported by the CPU.
  ```
  sudo apt install -y cpuid
  ```
- Run the following command to find out if _AES instruction_ is _true_:
  ```
  cpuid | grep -i aes | sort | uniq
  ```
- Run the following command to find out if _PCLMULDQ instruction_ is _true_:
  ```
  cpuid | grep -i pclmuldq | sort | uniq 
  ```

### Install GCC/G++: 
_Tested with version 8.3.1_
```
sudo apt-get install g++
```

### Install Python
_Tested with Python 2.7.17_
```
sudo apt-get install python
```

### Install numpy
```
sudo apt install python-pip
pip install numpy
```

### Install YASM (for MPIR)
```
sudo apt-get install yasm
```

### Install M4 (for MPIR)
```
sudo apt-get install m4
```

### Install Make 
```
sudo apt-get install make
```

### Install libboost-dev
```
sudo apt-get install libboost-dev
```

### Install libmysqlcppconn-dev. Tested version 1.1.9-1

```
$ sudo apt install libmysqlcppconn-dev 
```

### Install curl
```
sudo apt install curl
```

### Install Rustc
```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

To configure your current shell, run:
```
source $HOME/.cargo/env
```
To get nightly support use
```
rustup default nightly
```
Also we recommend installing wasm support via
```
rustup target add wasm32-unknown-unknown
```

### Install Cereal - _version 1.3.2_
Cereal is the library responsible for serializing and deserializing c++ objects 
To install follow the instructions located in the documentation of the [library](https://uscilab.github.io/cereal/quickstart.html)

Alternatively cereal can be installed using apt packages
```
sudo apt install libcereal-dev
```

### Install Catch2
This library is used to run C++ unit tests
1. Clone Catch2 repository (https://github.com/catchorg/Catch2.git)
2. Inside the repository, check out to "v2.x" branch
3. Run `cmake -Bbuild -H. -DBUILD_TESTING=OFF`
4. Run `sudo cmake --build build/ --target install`

## Detailed installation and setup instructions

Download the project from [GitHub](https://github.com/Crypto-TII/FANNG-MPC) and follow steps below:


### Step 1: Install mpir, openssl and crypto++
It is highly recommended to installed these dependencies inside your `/opt` directory, even though you could use the directory you prefer. 
Please note that the OpenSSL version differs from the one specified in the official Scale/Mamba documentation (`1.1.1g` instead of `1.1.0j`). 

```
$ mylocal="/opt"
$ mkdir -p ${mylocal}
$ cd ${mylocal}
$ 
$ #install MPIR
$ sudo curl -O 'http://mpir.org/mpir-3.0.0.tar.bz2'
$ sudo tar xf mpir-3.0.0.tar.bz2
$ cd mpir-3.0.0 || exit
$ sudo ./configure --enable-cxx --prefix="${mylocal}/mpir"
$ sudo make && sudo make check && sudo make install
$ 
$ #install OpenSSL 1.1.1g
$ cd $mylocal || exit
$ sudo curl -O 'https://www.openssl.org/source/openssl-1.1.1g.tar.gz'
$ sudo tar -xf openssl-1.1.1g.tar.gz
$ cd openssl-1.1.1g || exit
$ sudo ./config --prefix="${mylocal}/openssl"
$ sudo make && sudo make install
$ 
$ # install crypto++
$ cd $mylocal || exit
$ sudo curl -O https://www.cryptopp.com/cryptopp800.zip
$ sudo unzip cryptopp800.zip -d cryptopp800
$ cd cryptopp800 || exit
$ sudo make && sudo make install PREFIX=${mylocal}/cryptopp
```

### Step 2: Export mpir, openssl and crypto++ paths

Export the paths by copying the following lines at the end of your `~/.bashrc` configuration file.

```
# this goes at the end of your ~/.bashrc file
export mylocal="/opt"

# export OpenSSL paths
export PATH="${mylocal}/openssl/bin/:${PATH}"
export C_INCLUDE_PATH="${mylocal}/openssl/include/:${C_INCLUDE_PATH}"
export CPLUS_INCLUDE_PATH="${mylocal}/openssl/include/:${CPLUS_INCLUDE_PATH}"
export LIBRARY_PATH="${mylocal}/openssl/lib/:${LIBRARY_PATH}"
export LD_LIBRARY_PATH="${mylocal}/openssl/lib/:${LD_LIBRARY_PATH}"

# export MPIR paths
export C_INCLUDE_PATH="${mylocal}/mpir/include/:${C_INCLUDE_PATH}"
export CPLUS_INCLUDE_PATH="${mylocal}/mpir/include/:${CPLUS_INCLUDE_PATH}"
export LIBRARY_PATH="${mylocal}/mpir/lib/:${LIBRARY_PATH}"
export LD_LIBRARY_PATH="${mylocal}/mpir/lib/:${LD_LIBRARY_PATH}"

# export Crypto++ paths
export CPLUS_INCLUDE_PATH="${mylocal}/cryptopp/include/:${CPLUS_INCLUDE_PATH}"
export LIBRARY_PATH="${mylocal}/cryptopp/lib/:${LIBRARY_PATH}"
export LD_LIBRARY_PATH="${mylocal}/cryptopp/lib/:${LD_LIBRARY_PATH}"
```
**NOTE:** Do not forget to source `~/.bashrc`, after you have done this. 

<a id="step-3"></a>

### Step 3: Change CONFIG.mine
We now need to copy the file CONFIG in the main directory to the file CONFIG.mine. Then we need to edit
CONFIG.mine, so as to place the correct location of this ROOT directory correctly, as well as indicating where the
OpenSSL library should be picked up from (this is likely to be different from the system installed one which GCC
would automatically pick up). This is done by executing the following commands from the project's root directory:

```
cp CONFIG CONFIG.mine
echo "ROOT = <directory-where-you-cloned-the-project>" >> CONFIG.mine
echo "OSSL = ${mylocal}/openssl" >> CONFIG.mine
```

### Step 4: Final compilation
```
make progs
```

## Run a simple example

First, install the three party Shamir based variant, with default certificates and all parties running on
local host, executing the following commands:
```
cp Auto-Test-Data/1/* Data/
cp Auto-Test-Data/Cert-Store/* Cert-Store/
```

Then compile and run a simple program to make sure everything is working as expected.

```
./compile.sh Programs/tutorial
./Scripts/run-online.sh Programs/tutorial
```


## Alternative installation using nix-shell
In the documentation you will see quick install information
using `nix-shell` for installing the dependencies.

If you want to use this approach, install `nix-shell` if you haven't already

(you may need a re-login to update the environment variables).

```
curl -L https://nixos.org/nix/install | sh
```

Invoke `nix-shell` to get a fully ready development environment with all libraries installed. This will automatically
download all the dependencies and tools you need. After you have compiled the system you can compile a program in
the `Programs` directory by invoking

```
./compile.sh Programs/test_fix_array
```
