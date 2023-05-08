# mpc_graph_theory_lib (an `SCALE-MAMBA` implementation for Basic Graph Theory)

[![forthebadge](https://forthebadge.com/images/badges/as-seen-on-tv.svg)](https://github.com/Crypto-TII/mpc_graph_theory_lib)\
This repository includes (currently we only recommend experimentation):

* Basic random sampling using `Dijkstra` with cubic complexity from Aly et al.
* Improved `Dijkstra` with cuadratic complexity from Aly and Cleemput.
* Will include complete test files.

## Pre-requisites
* `numpy` 1.16 or above. (exclusively to __test__, which in this context means, execute [`test_graph.mpc`](../Programs/test_graph/test_graph.mpc)).

## Installation and Configuration
1. Download and configure `tii-mpclib`.
   
2. **That's it** the Dijkstra algorithm over arithmetic circuits (without depending on ORAM) are included in the [`mpc_graph_lib.py`](../Compiler/rabbit_lib.py):
1. You can now run them and check all tests are in <span style='color:green'>green.</span> (NOT in <span style='color:red'>red.</span>)

## Roadmap
We understand several Items are missing from this library. We have plans to populate the following:
* Complete set of tests for `dijkstra_optimized` and sorting funcitonality on `test_graph.mpc`.
* Complete the permutation functionality missing in the library. 

## License
## Contact Information:
If you have questions please contact any of the authors. Current repo maintainer is: [Abdelrahaman ALY](mailto:abdelrahaman.aly@tii.ae). 
### Authors: 
Abdelrahaman ALY
