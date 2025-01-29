[![forthebadge](https://forthebadge.com/images/badges/built-by-codebabes.svg)](https://www.tii.ae/cryptography)
[<img src="https://img.shields.io/badge/slack-@fanngmpc-blue.svg?logo=slack">](https://join.slack.com/t/fanng-mpc/shared_invite/zt-1v7c59129-B5Im3WXZ_rturvVV~MEEUA) 
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/Crypto-TII/FANNG-MPC/issues)

# WELCOME to FANNG-MPC - a SCALE-MAMBA FORK made by TII.
* Technology Innovation Institute Abu Dhabi*                  : TII 
* Framework for Artificial Neural Networks and General - MPC* : FANNG-MPC.  

We focus on the usability/deployability aspects of legacy **SCALE-MAMBA**. The new additions target Mamba rather than the rust compiler. We dedicate time to implement missing functionality and new libraries to manage ML related operations.

Remember, **this is a completely independent** fork from version 1.13, If you want to revise the original documentation of ```SCALE-MAMBA```, you can do so [here](https://nigelsmart.github.io/SCALE/Documentation.pdf).

## Who do I talk to?

This is a `Confidential Computing Team` contribution, in affiliation with various institutions. But don't worry we show you some cool ways to contact us below:

### Authors and Maintainers: 
There is no **I** in team, there is an **e**, and an **a**, but no **I**. Regardless **I** digress, here are the names of all the peeps who made this possible (in alphabetic order):

* Najwa Aaraj
* Abdelrahaman Aly
* Tim Güneysu
* Chiara Marcolla
* Johannes Mono
* Jose Angel Nieda
* Ivan Santos
* Mireia Scholz
* Eduardo Soria Vasquez
* Victor Sucasas 
* Ajith Suresh 
  
### How to Contribute:

If you would like to contribute, which you are welcome to do so, please follow this simple rules:

* Follow the coding style of the framework. 
* Include test files. 
* Be sure all tests available pass. They run in the same way they used to in ```SCALE-MAMBA```.
* Submit a PR, stating a Description, and what tests are concerned.

### Report Issues
We appreciate your inputs. If you have find some issues and wish to report them, please don't forget the following:

* Please Create a Github Issue/Jira ticket.
* Add a description of the issue, including inputs and outputs and configuration (prime size, protocols, players and parameters if modified).  
* Include a code example of your issue. Be sure is reproducible. 
  
If you want to have a chat or have questions, you can also find us on this [Slack Channel](https://fanng-mpc.slack.com). If you are ol'school and would like to mail us, you can do so at [fanngs-mpc@tii.ae](fanng-mpc@tii.ae). 

## How to Cite it?
This work is under submission, until then we recommend you cite us as follows:

```bibtex 
@article{AAGMMPSSSSS23,
  author = {Najwa Aaraj and Abdelrahaman Aly and Tim Güneysu and Chiara Marcolla and Johannes Mono and Rogerio Paludo and Iván Santos-González and Mireia Scholz and Eduardo Soria-Vazquez and Victor Sucasas and Ajith Suresh},
  title = {{FANNG-MPC: Framework for Artificial Neural Networks and Generic MPC}},
  journal = {IACR Transactions on Cryptographic Hardware and Embedded Systems},
  year = {2025},
  volume = {2025},
  number = {1},
  pages = {1--36},
  doi = {10.46586/tches.v2025.i1.1-36},
  url = {https://philosophymindscience.org/index.php/TCHES/article/view/11920}
}
```

Besides what you can find above, the framework is complemented by following contributions:

* [Finding and Evaluating Parameters for BGV](https://eprint.iacr.org/2022/706). We use it to define the underlying ```BGV``` parameters.
* [Through the Looking-Glass: Benchmarking Secure Multi-Party Computation Comparisons for ReLU's](https://eprint.iacr.org/2022/202). We use it to produce our ReLUs. 
* [Implementing and Optimizing Matrix Triples with Homomorphic Encryption](https://eprint.iacr.org/2023/593). We use it to create our Matrix Triples. 
* [Improving and Automating BFV Parameters Selection: An Average-Case Approach](https://eprint.iacr.org/2023/600). We also use some elements from here on our parameter calculations. 
  
If you are interested on only one of the aspects above, we do not mind you citing that work instead.

## The How's (Before You Start):
Our documentation provides answers to the questions you might have If you have questions about legacy code, you can check the **SCALE-MAMBA official documentation**.

On the Where's (for now): 

* We only support up to Ubuntu 20.4
* We only support Intel x86 Architectures with the correct collection of instructions (check our [installation guide](installation/installation-guide.md)).
On the How's we have included:

* A **new [installation guide](installation/installation-guide.md)** that includes instructions for an automatized install, manual install, and nix support execution. 
* If you are not a read the docs kinda person, don't you worry, we gotcha! We provided you with a script that automatize the installation. Just run `installation/setup-project.sh` and it will quickly install and set the project up (It will also use the Default Example 1 from [Auto-Test-Data](Auto-Test-Data/), including setting up your Certificates). More details can be found, on the link above. 

We support DB's now. Hence, we have added a **new configuration file** to the Data folder: The Storage config file. We have added them to all the [Auto-Test-Data](Auto-Test-Data/) example setups, persisting information on files.

Learn more about storage config files [here](MD-Files/storage-config-file.md). 

### Setting up your own MySQL
We use `MySql` as the default DB Engine. Our architecture is flexible enough to allow you incorporate different engines. They are used by the Input/Output database system and the Offline Garbling feature. At the moment however we only support `MySql`. 

**NOTE:** There are different DB models for different target applications. 

Learn how to set the databases up [here](MD-Files/database-setup.md).
* **
## The New Features (Release Notes):
This list includes some new features and solving some old problems:
### __Updated Input/output System__
We have added file and DB support to the default I/O. In this library, the default Input/Output system is now `input_output_db.cpp`. This is thought for legacy users who are used to work with channels and would like to use DB's. Note that all our new functionalities manage DB's differently. 

You can learn more about the new Input/Output features [here](MD-Files/input-output.md).

### __Offline Garbling__
We are able to support the production and persistence of Garbling Circuits. They can be produced in advance, stored, and used later.  We have included 3 new cool instructions to achieve that, namely `OGC`, `LOADGC` and `EGC`.

You can learn more about Offline Garbling [here](MD-Files/offline-garbling.md).

#### __Garbled Circuits Generation Script__
To ease their use, we have also incorporated a new bash [Script](Scripts/gc-gen.sh) to generate and store Garbled circuits into our DB. It uses the instructions above. you can invoke the script via:

```bash
./Script/gc-gen.sh
```

### __Optimizations for Big Vectors__
Old versions of `SCALE-MAMBA`, 1.14 and below, cannot handle opening vectors above approx 100K items. Hence, we have included a connection pool and 2 new instructions, that act like the old `POpen_Start` and `POpen_Stop`. 
**We call them `POpen_Start_Batched` and `POpen_Stop_Batched`** .

You can learn more about how to work with huge vectors `POpen_Start` and `POpen_Stop` workaround [here](MD-Files/open-huge-vectors-workaround.md).

### __Streamlined Memory Management__
Ever noticed your `SCALE-MAMBA` programs gobbling up more memory than they should? Well, in versions 1.14 and below, this is a common hiccup. The culprit? Misjudged register sizes. To fix this, we modify the [compile-mamba.py](compile-mamba.py) script to correct overblown register allocations. 

You can learn more about the modification we made to [compile-mamba.py](compile-mamba.py) [here](MD-Files/streamlined-memory-management.md).


### __Parallel Truncation (Vectorized)__
Adding a connection pool has also allowed us to implement a vectorized version of the truncation method. One of the biggest bottlenecks in ML implementations. This accelerates compilation and execution times in a way that was simply not possible before. You can find the new truncation methods in the [AdvInteger.py](Compiler/AdvInteger.py) library. The method call is the following:

* `TruncPr_parallel(a, k, m, kappa=40)`: work in the same as legacy `TruncPR` except that `a` is a vectorized `sint`.
* ` TruncPr_exp_parallel(a, k, m, exp_r_dprime, exp_r_prime, kappa=40)`: Slightly optimized version of the method above. In case you have several invocations of the parallel truncation, you can use this method and send the expansion vectors for r_dprime and r_prime. Note that `TruncPr_parallel` recalculates these values at every call. 

**NOTE:** If you want to to learn how to mix vectorized truncations with comparisons, let's say, for ReLUs.. You can check out our [relu_lib](MD-Files/relu-lib.md) documentation. These vectorizations are specially helpful when compilation optimizations are not possible.


### __Mixed Circuit for ReLUs__
You can implement comparison methods using mixed circuits (arithmetic and boolean). In our case, we have implemented the constant round mechanisms (FT Setups) from [Through the Looking GLass](https://eprint.iacr.org/2022/202).

#### __64 bit circuits for ReLUs__
As part of the support for mixed circuit ReLUs, we include new 64 bit circuits for comparisons, inspired by [Rabbit](https://eprint.iacr.org/2021/119) and [Through the Looking Glass](https://eprint.iacr.org/2022/202).

**NOTE:** If wishing to recompile the basic 64 bit circuits then follow the old instructions from **SCALE-MAMBA**.

We are using a specialized circuit, compiled for Less Than Equal in 2 versions:

* ```Circuits.ONLY_ANDS``` Yosys generated circuit only using HW optimizations ```ref: 66001```.
  
* ```Circuits.ANDS_XORS``` To  be done. Yosys generated circuit including Xors ```ref: 66002```.
>We are currently working with HW Researchers on a joint paper about how to mix Yosys and GC's. STAY TUNED!

You can learn more about our ReLUs [here](MD-Files/relu-lib.md).


### __Means to Load Preprocessed Materials From DBs__
RAM Memory has its limitations, but here's the good part: parties can actually pre-compute the stuff they need and save it in their own DB. And guess what? It's totally safe to do so! This approach allows parties to load only the specific material they need into the RAM, resulting in improved performance.

Now, we've made it easy for you by providing a method to do this for various tasks, such as matrix triples and bounded randomness. We've even come up with a new set of instructions for it. The best part is, it's super user-friendly and uses a novel and unified way to utilize DBs within the framework. So, you can have your cake and eat it too – optimizing your performance without much hassle! 

You can learn more about our feature to load preprocessed materials [here](MD-Files/preprocessing-material-loading.md).

>Do not forget, `Trusted Dealers` are not real, no matter what people say. 

### __Conversion of Preprocessed Materials in the Dealer Model__

We have enhanced SCALE-MAMBA by separating pre-processing from the online phase and introducing a dealer model. The challenge now is utilizing these preprocessed materials provided by different dealers. To address this, we have designed a converter module to streamline the transition. Essentially, this module converts preprocessed materials from one format used by the dealers to another format suitable for online evaluation with a different set of parties. 

Given its broader application, we have decided to host the converter module in a separate repository. You can find it [here](https://github.com/Crypto-TII/FANNG-MPC-Converter/).

### __A New Graph Theory Library__

As a little extra, we have also implemented some graph theory protocols. Namely the State of the Art of Single Source Shortest Path Exploration for MPC. This is based on the work by [Aly and Cleemput](https://eprint.iacr.org/2017/971). 

You can learn more about them [here](MD-Files/mpc-graph-lib.md).

__NOTE:__ If it's in the Compiler folder and has **lib** as its suffix, it's new!


### __A Library for ML blocks -> convolutions and fully connected layers__

We have put together a new library with ML functionalities to deploy convolutional layers and fully connected layers. The library also contains several functionalities to manipulate matrixes. 

Additionally, we included two new bytecodes in SCALE to perform array summation in register values.

All functions are described [here](MD-Files/matrix_lib.md).


### __A Library for ML blocks -> folding layers, input/batch processing, and output layers__

This library contains new ML functionalities to implement: i) folding layers (i.e. pooling); ii) batch normalization; iii) input normalization & standardization; iv) and some output layers such as softmax.

All functions are described [here](MD-Files/folding_lib.md).


### __How can you build your own Private Neural Network?__

[Here](MD-Files/oblivios_nn.md) we explain how to construct an oblivious neural network using the libraries detailed above. Specifically, it explains how to construct a CNN to classify CIFAR-10 data. The CNN architecture is included in a diagram, and PyTorch code is included in the repository [here](./pytorch-implementations/Pruned-Resnet/), and the private version is included [here](./Programs/test_obliv_nn_pruned_resnet/). 

If you need guidance on generating the required input and output files, along with the essential parameters, using the [pytorch Pruned Resnet implementation](./pytorch-implementations/Pruned-Resnet/), as well as compiling and running the [FANNG-MPC oblivious-CIFAR10 test program](./Programs/obliv_nn_pruned_resnet/), you can refer to this [step-by-step guide](MD-Files/pruned-resnet-step-by-step-guide.md).


There is a smaller CNN for MNIST also implemented privately in the previous repo, and the Pytorch code for that one is [here](./pytorch-implementations/Lenet/).

Note that our implementation uses all the functionalities explained above: I) ReLUs, ii) linear transformations; iii) folding layers; iv) new I/O functions with MySQL connection. However, for triple generation we use fake ones (i.e. secret shares of zero matrixes).

Note that the quantity of RAM required for compilation and execution of this network is not available in a normal personal computer. If you want to try in your own laptop, we recommend deploying the network for MNIST instead. If you need guidance on generating the required input and output files, along with the essential parameters, using the [pytorch LeNet implementation](./pytorch-implementations/Lenet/), as well as compiling and running the [FANNG-MPC oblivious-LeNet test program](./Programs/test_obliv_nn_lenet/), you can refer to this [step-by-step guide](MD-Files/lenet-step-by-step-guide.md).


