# FANNG-MPC Installation Guide

This guide provides instructions for setting up the [FANNG-MPC](https://github.com/Crypto-TII/FANNG-MPC) project, including installing dependencies and configuring the environment. __The project has been tested on Ubuntu versions up to 20.04__.

## Prerequisites
<a id="cpu-support"></a>
### CPU Requirements
The FANNG-MPC framework requires a CPU with **AES-NI** and **PCLMULQDQ** instruction support. If your CPU does not support these instructions, the framework will not function as expected.

To check your CPU's compatibility, follow these steps:

1. **Install the `lscpu` tool:**  
   This utility provides detailed information about your CPU, including supported instructions. If necessary, update your package list before proceeding:
   ```bash
   sudo apt update
   sudo apt install -y lscpu
   ```

2. **Verify AES-NI support:**  
   Run the following command to check if the AES instruction is supported:
   ```bash
   lscpu | grep -i aes 
   ```

3. **Verify PCLMULQDQ support:**  
   Run the following command to check if the PCLMULQDQ instruction is supported:
   ```bash
   lscpu | grep -i pclmulqdq

   ```

If both instructions are supported, you can proceed with the installation. Please note you can do the same with  `cpuid` if you prefer.

__NOTE:__ Please take into consideration that those instructions are only present on x86 architectures. At the moment we do not offer support for ARM chipsets.

---

## Quick Installation Script

To simplify the setup process, a script is provided to automatically install dependencies and configure the project.

1. Navigate to the root folder of the repository.
2. Run the setup script:
   ```bash
   source installation/setup-project.sh
   ```

The script consists of five stages, and upon successful execution, you will see the message:  
`------ FANNG-MPC IS NOW READY TO USE ------`.

### Important Notes:
- The script may fail if a required resource (e.g., a link) is unavailable. If this happens, manually execute the script’s steps starting from where it failed, one command at a time.
- Refer to the [Manual Installation](#manual-installation) section for detailed instructions.

After a successful setup, you can begin using the FANNG-MPC framework.

---

## Run a Simple Example

To confirm that the framework is working as expected, compile and run a simple example from the root folder:

1. Compile the example program:
   ```bash
   ./compile.sh Programs/tutorial
   ```

2. Run the program online:
   ```bash
   ./Scripts/run-online.sh Programs/tutorial
   ```

---

## Manual Installation
If the quick installation script fails or you prefer a manual setup, follow the steps below to install the [FANNG-MPC](https://github.com/Crypto-TII/FANNG-MPC) framework manually.

### Clone the Repository and Navigate to the Project Directory
Download the project from GitHub and move into the project directory using the following command:  
```bash
git clone https://github.com/Crypto-TII/FANNG-MPC.git
cd FANNG-MPC
```

The installtion comprises of five stages, detailed below:

### Stage 1: Installing Necessary Dependencies 

#### Install GCC/G++: 
_Tested with version 8.3.1_
```
sudo apt-get install g++
```

#### Install YASM (for MPIR)
```
sudo apt-get install yasm
```

#### Install curl
```
sudo apt-get install curl
```

#### Install wget
```
sudo apt-get install wget
```

#### Install libboost-dev
```
sudo apt-get install libboost-dev
```

#### Install libmysqlcppconn-dev. Tested version 1.1.9-1

```
$ sudo apt-get install libmysqlcppconn-dev 
```

#### Install M4 (for MPIR)
```
sudo apt-get install m4
```

#### Install Make 
```
sudo apt-get install make
```

#### Install unzip 
```
sudo apt-get install unzip
```

#### Install cmake 
```
sudo apt-get install cmake
```

#### Install Python and its dependencies
_Tested with Python 2.7.17_
```
sudo apt-get install -y python
```

Install pip and numpy
```
sudo curl -O 'https://bootstrap.pypa.io/pip/2.7/get-pip.py'
python2 get-pip.py
python2 -m pip install numpy
rm -f get-pip.py
```

#### Install Rustc
```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
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
Finally, source rustc
```
source rustup
```

#### Install Cereal - _version 1.3.2_
Cereal is the library responsible for serializing and deserializing c++ objects. 
To install, follow the instructions located in the documentation of the [library](https://uscilab.github.io/cereal/quickstart.html).

Alternatively cereal can be installed using apt packages.
```
sudo apt install libcereal-dev
```

#### Install mpir, openssl, crypto++ and Catch2
It is highly recommended to installed these dependencies inside your `/opt` directory, even though you could use the directory you prefer. 
Please note that the OpenSSL version differs from the one specified in the official Scale/Mamba documentation (`1.1.1g` instead of `1.1.0j`). 

```
$ mylocal="/opt"
$ mkdir -p ${mylocal}
$ cd ${mylocal}
$ 
$ #install MPIR
$ sudo curl -O 'https://src.fedoraproject.org/lookaside/extras/mpir/mpir-3.0.0.tar.bz2/sha512/c735105db8b86db739fd915bf16064e6bc82d0565ad8858059e4e93f62c9d72d9a1c02a5ca9859b184346a8dc64fa714d4d61404cff1e405dc548cbd54d0a88e/mpir-3.0.0.tar.bz2'
$ sudo tar xf mpir-3.0.0.tar.bz2
$ cd mpir-3.0.0 
$ sudo ./configure --enable-cxx --prefix="${mylocal}/mpir"
$ sudo make && sudo make check && sudo make install
$ sudo rm $mylocal/mpir-3.0.0.tar.bz2
$ 
$ #install OpenSSL 1.1.1g
$ cd $mylocal
$ sudo wget 'https://www.openssl.org/source/openssl-1.1.1g.tar.gz'
$ sudo tar -xf openssl-1.1.1g.tar.gz
$ cd openssl-1.1.1g
$ sudo ./config --prefix="${mylocal}/openssl"
$ sudo make && sudo make install
$ 
$ # install crypto++
$ cd $mylocal 
$ sudo wget https://github.com/weidai11/cryptopp/releases/download/CRYPTOPP_8_2_0/cryptopp820.zip
$ sudo unzip cryptopp820.zip -d cryptopp820
$ cd cryptopp820 
$ sudo make && sudo make install PREFIX=${mylocal}/cryptopp
$ sudo rm $mylocal/cryptopp820.zip
$
$ cd $mylocal 
$ sudo wget https://github.com/catchorg/Catch2/archive/refs/heads/v2.x.zip --no-check-certificate
$ sudo unzip v2.x.zip
$ cd Catch2-2.x 
$ sudo cmake -Bbuild -H. -DBUILD_TESTING=OFF
$ sudo cmake --build build/ --target install
$ sudo rm $mylocal/v2.x.zip
```

### Stage 2: Export mpir, openssl and crypto++ paths

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
```bash
 source ~/.bashrc
```

<a id="step-3"></a>

### Stage 3: Change CONFIG.mine
We now need to copy the file CONFIG in the main directory to the file CONFIG.mine. Then we need to edit
CONFIG.mine, so as to place the correct location of this ROOT directory correctly, as well as indicating where the
OpenSSL library should be picked up from (this is likely to be different from the system installed one which GCC
would automatically pick up). This is done by executing the following commands from the project's root directory:

```
cp CONFIG CONFIG.mine
sed -i "s|ROOT = /users/<user>/<repo>|ROOT = <directory-where-you-cloned-the-project>|" "CONFIG.mine"
sed -i "s|OSSL = /opt|OSSL = /opt/ossl|" "CONFIG.mine"
```

### Stage 4: Final compilation
```
make progs
```

### Stage 5: Selecting a Protocol Variant

The project supports **27 protocol variants**, numbered 1 through 27. To install the **three-party Shamir-based variant (variant 1)** with default certificates and all parties running on the local host, execute the following commands:

```bash
cp Auto-Test-Data/1/* Data/
cp Auto-Test-Data/Cert-Store/* Cert-Store/
```

These commands copy the necessary configuration files for variant 1 into the appropriate directories.


---


## Alternative installation using nix-shell
In the documentation, you will see quick install information
using `nix-shell` for installing the dependencies.

If you want to use this approach, install `nix-shell` if you haven't done it already.

(you may need a re-login to update the environment variables).

We suggest you to follow these steps:

### Install `nix-shell`:

First, download and install nix:
```
curl -L https://nixos.org/nix/install | sh
```

Once the installation is finished, please enable nix to your shell:

```
. ~/.nix-profile/etc/profile.d/nix.sh
```

You can make these changes permanent by adding nix to your `.bashrc`:
```
echo ". ~/.nix-profile/etc/profile.d/nix.sh" >> ~/.bashrc
```

Finally, don't forget to always source your `/.bashrc`:
```
source ~/.bashrc
```
### Configuring `FANNG-MPC` in your `nix-shell`: 
First, invoke `nix-shell` to get a fully ready development environment with all libraries installed via: 

```
cd <path_to_fanng>/FANNG_MPC
nix-shell
```
Create a `CONFIG.mine`: 
```
cp CONFIG CONFIG.mine
```
In `CONFIG.mine`, erase the `OSSL` variable and set `ROOT = ..`.

### Running `FANNG-MPC`:
You have to simply follow __[stage 4](#stage-4-final-compilation)__ and __[stage 5](#stage-5-selecting-a-protocol-variant)__ as described above. This will allow you to compile `FANNG-MPC` and configure your secret sharing scheme.

After you have finished both, you can now compile and run any program in
the `Programs` directory by invoking (we use `tutorial` as an example):

```
cd <path_to_fanng>/FANNG_MPC
./compile.sh Programs/tutorial
./Scripts/run-online.sh Programs/tutorial
```

Please note the above needs to be executed from `<path_to_fanng>/FANNG_MPC`.

And that's it! Thanks for using `FANNG-MPC`.