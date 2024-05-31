# beyond_rabbit (an MPC `SCALE-MAMBA` Implementation)
[![forthebadge](https://forthebadge.com/images/badges/as-seen-on-tv.svg)](https://github.com/Crypto-TII/beyond_rabbit)

A __Rabbit through the mirror__ full implementation. We include support for ReLUs. This release includes:

* Basic random sampling using `dabits` in MAMBA. 
* VHDL Circuits and Bristol Fashion Files.
* Constant Round implementations of Rabbit and Catrina comparison protocols from Through the Mirror.  
* A `relu` implementation, and a complementing library.
* Easy to use test files, to verify your installation/configuration is correct.

This is to the best of our knowledge the first implementation of any support for the `dabits` instruction in `mamba.`

## Pre-requisites (already included in tii-mpclib installation)
* `numpy` 1.16 or above (it is used exclusively to __test__, which in this context means, execute [`test_relu.mpc`](beyond_rabbit/test_relu/test_relu.mpc)).

## Installation and Configuration
1. Download and configure `tii-mpclib`:

2. **That's it.** All 6 comparison modes (class `Mode`) are included in the [`rabbit_lib.py`](../Compiler/rabbit_lib.py):

   * `rabbit_slack`: it receives `sint` and utilizes slack in conjunction with `rabbit` classic. It returns a `sint` containing either 0 or 1.
   * `rabbit_list`: it receives `sint` and utilizes a rejection list in conjunction with `rabbit` classic. It returns a `sint` containing either 0 or 1.
   * `rabbit_fp`: it receives `sint` and assumes a prime close to a power of 2 in conjunction with `rabbit` classic. The method requires you to define if the approximation is from below or above. It returns a `sint` containing either 0 or 1.
   * `rabbit_conv:`it receives `sint` and `2 to the k` domains composed bitwise as proposed by `rabbit`. It returns a `sint` containing either 0 or 1.
   * `rabbit_less_than:` it receives `sint` and utilizes a boolean less-than circuit. It returns a `sint` containing either 0 or 1.
   * `dabits_ltz`: it receives `sint` and utilizes slack generated via `dabits` for `Catrina and De Hoogh`. It returns a `sint` containing either 0 or 1.

3. You can now run the test file and check all tests are in <span style='color:green'>green.</span> (NOT in <span style='color:red'>red.</span>)

## Special Features:
**Parallel Truncation with ReLU.** We observe that many machine learning functions, such as neural networks, are constructed as a series of multiplication or convolution operations, followed by the application of ReLU. To leverage this structure effectively, we merge a set of sequential truncations with the ReLU operation, leading to enhanced efficiency. To achieve this, we introduced an additional comparison mode in the [`rabbit_lib.py`](../Compiler/rabbit_lib.py):

   * `dabits_trunc_ltz`: it receives `sint` along with a public int `batch` and reuses the mask for truncation of the `batch` number of previous multiplications and the `dabits_ltz` comparison. It returns two `sint` values: one represents the truncated input, and the other is the comparison bit. 
   
   **NOTE:** The facades have been built in a way that allows for the vectorization of inputs. This feature is extremely important when compiling without optimizations, which is a common issue in Machine Learning.

## Mode of Use:
We included facade methods that can be parametrized with the rabbit mode desired. This is true for simple LTZ tests and for ReLUs:
* `rabbit`: __on [rabbit_lib.py](../Compiler/rabbit_lib.py)__ it receives `sfix` inputs and a comparison mode. It returns the comparison using the specified mode.
* `rabbit_sint`: __on [rabbit_lib.py](../Compiler/rabbit_lib.py)__ it receives `sint` inputs and a comparison mode. It returns the comparison using the specified mode.
* `relu`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives a `sint` relu mode input and a gradient mode (optional). It returns a `relu_response` object using the specified mode.
* `relu_sfix`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives a `sfix` relu mode input and a gradient mode (optional). It returns a `relu_response` object using the specified mode.
* `relu_2d`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives either `sint` or `sfix` matrix (2-dimensional vector) input and a gradient mode (optional). It returns a `relu_response` matrix (2-dimensional vector) object using the specified mode.
* `relu_3d`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives either `sint` or `sfix` matrix (3-dimensional vector) input and a gradient mode (optional). It returns a `relu_response` matrix (3-dimensional vector) object using the specified mode.

* `trunc_LTZ`: __on [rabbit_lib.py](../Compiler/rabbit_lib.py)__ it receives a `sint` input, public parameter `batch` and a comparison mode. It returns the batch-truncated input along with the comparison result using the specified mode.
* `relu_trunc`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives a `sint` relu mode input, public parameter `batch` and a gradient mode (optional). It performs ReLU over the batch-truncated input and returns a `relu_response` object using the specified mode.
* `relu_trunc_sfix`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives a `sfix` relu mode input, public parameter `batch` and a gradient mode (optional). It performs ReLU over the batch-truncated input and returns a `relu_response` object using the specified mode.
* `relu_trunc_2d (not vectorized)`:__on [relu_lib.py](../Compiler/relu_lib.py)__ it receives either `sint` or `sfix` matrix (2-dimensional vector) input, public parameter `batch` and a gradient mode (optional). It performs ReLU over the batch-truncated input matrix and returns a `relu_response` matrix (2-dimensional vector) object using the specified mode.
* `relu_trunc_3d (not vectorized)`: __on [relu_lib.py](../Compiler/relu_lib.py)__ it receives either `sint` or `sfix` matrix (3-dimensional vector) input, public parameter `batch` and a gradient mode (optional). It performs ReLU over the batch-truncated input matrix and returns a `relu_response` matrix (3-dimensional vector) object using the specified mode.

### Special Configurations: 
The library can be configured to use a different `Base Circuit` by default or any `Garbling Online-Offline` mode. It provides 2 global variables for that purpose, namely (with their actual values):
```
DEFAULT_CIRCUIT = Circuit.ONLY_ANDS
DEFAULT_GARBLING = Garbling.ONLINE_GARBLING
```
Depending on the function, you can always override the library default and use a specific configuration when the function is invoked. These kind of configurations are a bit more complicated and are recommended for expert users. You can check [rabbit_lib.py](../Compiler/rabbit_lib.py) for more details. 

If you decide to use a different circuit, you have 2 options:

* **Change the default**. This would affect all executions of the functions above.
* **Parametrize the circuit id**. You can do this by using the Circuit class on the native methods that use circuits, for example:
``` python
a = sint(5)
c = dabits_LTZ(a, circuit=Circuit.ANDS_XORS)
```
Please be advised you cannot directly parametrize the circuit on the `facade` functions. Hence, we believe this kind of invocations are better left for advance users.

**NOTE:** Finally, you can also parametrize comparison related `facade`'s with an specific `rabbit_lib` mode as follows: 
```python
a = sint(5)
b = sint(1)
c = dabits_LTZ(a,b mode = Mode.RABBIT_LIST)
```
On the other hand, `relu_lib` related `facade`s cannot be parametrized this way. They use `Mode.dabits_LTZ` by default.

## Contact Information:
If you have questions please contact any of the authors. Current repo maintainer is: [Abdelrahaman ALY](mailto:abdelrahaman.aly@tii.ae). 
### Authors: 
* Abdelrahaman ALY
* Victor SUCASAS
* Kashif NAWAZ
* Eugenio SALAZAR
* Ajith SURESH
