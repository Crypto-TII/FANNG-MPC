### __Streamlined Memory Management__
To address the misjudged register sizes, we modify the [compile-mamba.py](../compile-mamba.py) script. At a high level, the modified script uses the actual register sizes from the compiler and overwrites the register sizes in the program with them, ensuring better memory utilization. Specifically, it extracts the register sizes using the `reg_counter` from the program tape during compilation ([compile-mamba.py](../compile-mamba.py)), updating them in [Processor.cpp](../src/Processor/Processor.cpp). 

Furthermore, the script stores these register sizes for future use by saving them in a `.log` file within the corresponding program folder. This approach eliminates the need to re-compile the same program in the future. Instead, users can update register sizes in [config.h](../src/config.h) using values from the log file and by setting the `ignore_memory_sizes` flag to `1` in [config.h](../src/config.h). The relevant values in [config.h](../src/config.h) are as follows:

```c
/* This is used to fix the memory bug associated with 
 * existing code. If set to 1, it enables a script to
 * overwrite the maximum register sizes with values below.
 */
#define ignore_memory_sizes 0

#define REG_MAXB_SIZE 0
#define REG_MAXC_SIZE 0
#define REG_MAXI_SIZE 0
#define REG_MAXP_SIZE 0
```

It's important to note that while this script offers a workaround solution, it has a limitation: it cannot leverage register optimizations performed by the scasm.

By seamlessly integrating this script into the compilation process, you can optimize your program's memory footprint. For this, simply append the `-F` flag during compilation, as shown below:
```bash
./compile.sh -F <program name>
```
For more details on compilation, see our [installation guide](../installation/installation-guide.md).