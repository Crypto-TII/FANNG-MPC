# beyond_rabbit (an MPC `SCALE-MAMBA` Implementation)
[![forthebadge](https://forthebadge.com/images/badges/as-seen-on-tv.svg)](https://github.com/Crypto-TII/beyond_rabbit)

A __Rabbit through the mirror__ full implementation. We include support for RelUs. This release includes:

* Basic random sampling using `dabits` in MAMBA. 
* VHDL Circuits and Bristol Fashion Files.
* Constant Round implementations of Rabbit and Catrina comparison protocols from Through the Mirror.  
* A `relu` implementation, and a complementing library.
* Easy to use test files, to verify your installation/configuration is correct.

This is to the best of our knowledge the first implementation of any support for the `dabits` instruction in `mamba.`

## Pre-requisites (already included in tii-mpclib installation)
* `numpy` 1.16 or above. (it is used exclusively to __test__, which in this context means, execute [`test_relu.mpc`](beyond_rabbit/test_relu/test_relu.mpc)).

## Installation and Configuration
1. Download and configure `tii-mpclib`:

2. **That's it** All 5 comparison modes are included in the [`rabbit_lib.py`](../Compiler/rabbit_lib.py):
   
    * `rabbit list`: it receives `sint` and utilizes a rejection list in conjuction with `rabbit` classic. It returns a `sint` containing either 0 or 1.
   * `rabbit_fp`: it receives `sint` and assumes a prime close to a power of 2 in conjuction with `rabbit` classic. The method requires you to define if the approximation is from below or above. It returns a `sint` containing either 0 or 1.
   * `rabbit_slack`: it receives `sint` and utilizes slack in conjuction with `rabbit` classic. It returns a `sint` containing either 0 or 1.
   * `dabits_ltz`: it receives `sint` and utilizes slack generated via `dabits` for `Catrina and De Hoogh`. It returns a `sint` containing either 0 or 1.
   * `rabbit_conv:`it receives `sint` and `2 to the k` domains composed bitwise as proposed by `rabbit`. It returns a `sint` containing either 0 or 1.
3. You can now run the test file and check all tests are in <span style='color:green'>green.</span> (NOT in <span style='color:red'>red.</span>)

## Mode of Use:
We included facade methods that can be parametrized with the rabbit mode desired. This is true for simple LTZ tests and for ReLUs:
* `rabbit`: __on [rabbit_lib.py](../Compiler/rabbit_lib.py)__ it receives `sfix` and a comparison mode. It returns the comparison using the specified mode.
* `rabbit_sint`: __on [rabbit_lib.py](../Compiler/rabbit_lib.py)__ it receives `sint` and a comparison mode. It returns the comparison using the specified mode.
* `relu`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives either `sint` of `sfix` a relu mode and input type and gradient mode (optional). It returns a `relu_response` object using the specified mode.
* `relu_2d`:__on [relu_lib.py](../Compiler/relu_lib.py)__ it receives either `sint` of `sfix` matrix (2-dimensional vector), a relu mode and input type and gradient mode (optional). It returns a `relu_response` matrix (2-dimensional vector) object using the specified mode.
* `relu_3d`: it receives either `sint` of `sfix` matrix (3-dimensional vector), a relu mode and input type and gradient mode (optional). It returns a `relu_response` matrix (3-dimensional vector) object using the specified mode.

## Contact Information:
If you have questions please contact any of the authors. Current repo maintainer is: [Abdelrahaman ALY](mailto:abdelrahaman.aly@tii.ae). 
### Authors: 
* Abdelrahaman ALY
* Victor SUCASAS
* Kashif NAWAZ
* Eugenio SALAZAR
