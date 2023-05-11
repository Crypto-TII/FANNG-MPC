# MPCLib Offline Garbling

## Introduction
Garbling offline is a new functionality for MPCLib that allows garbling several circuits during the offline phase
of an MPC Protocol to reduce the computation time of a circuit during the online phase. 

The circuits are garbled and then persisted, so they can be later loaded into memory and finally evaluated.
For now, the circuits can be persisted using in **MySQL databases** or the **file system**.

## How it works
Three new MAMBA instructions have been added to MPCLib framework: OGC, LOADGC and EGC. Notice that
**these instructions are only supported for full-threshold setups.**

### OGC
This instruction is in charge of garbling and persisting the circuits for each player. 
It receives two arguments: 
- **Circuit number**. More information about circuits and each of their identification numbers could be found [here](src/GC/Base_Circuits.cpp)
- **Number of times** the circuit will be garbled

For example:
```
less_eq_circuit=66001
number_of_circuits=2
OGC(less_eq_circuit, number_of_circuits)
```

It will persist the garbled circuit depending on the selected storage type. 

- For MySQL databases all the garbled circuits will be stored in the database specified in the storage config file, 
notice that the database system should be already set up. 

- For filesystem an output file in JSON format will be generated, following this 
nomenclature: `<circuit number>-<iteration>-P<player-number>.json`. It will contain the information of the garbled circuit and will be located in the **Garbled_Circuits** directory.

A code example can be found [here](../Programs/test_OGC_LOADGC_EGC/test_OGC_LOADGC_EGC.mpc). 


### LOADGC
This instruction is an intermediate operation between ```OGC``` and ```EGC```. It receives two arguments: the circuit number and the number of circuits to be loaded into memory.

It will read the garbled circuits from the selected storage system and store then in memory, so they can later be consumed by the EGC instruction.

A code example can be found [here](../Programs/test_EGC_float/test_OGC_LOADGC_EGC_float.mpc)

### EGC
This instruction evaluates a garbled circuit. It receives one argument: the circuit number. It will only evaluate one circuit at a time. 

As in a queue, the first garbled circuit that had been loaded in memory using ```LOADGC``` a will be the one to be evaluated.

**Make sure to load as many circuits as you need, otherwise an exception will be thrown.**

A code example can be found [here](../Programs/test_OGC_LOADGC_EGC_float/test_OGC_LOADGC_EGC_float.mpc)

### Automated Circuit Creation
We have incorporated an automated mechanism to invoke ```OGC```. It can be parametrized directly on the.mpc file [here](../Programs/test_gc_gen/test_gc_gen.mpc). 

As an alternative, the user can invoke an interactive Script to avoid the direct manipulation of the  ```.mpc``` file, it can be invoked via ```./Scripts/gc-gen.sh```. The user has to follow the instructions provided by the Script together with the inputs it requests. The script can be found [here](../Scripts/gc-gen.sh). 

## Choicebits files
In order to evaluate a circuit that has been garbled in a different program execution, an additional config file per player in the /Data directory is required as well: `choicebits-P$playernumber`.

These files can be found in the preconfigured folders for full threshold setups in [Auto-Test-Data](Auto-Test-Data) (subdirectories from 18 to 21).
They are also generated when running `Setup.x`, only for full-threshold set-ups as well.

If your choicebits files are empty, new choicebits will be generated and persisted in the files once you run any mpc program.

### Why?
The choicebits generated in [aBitFactory](src/OT/aBitFactory.cpp)'s initialization method need to be the same for both the garbling and the evaluation process.
If garbling process wants to be executed in a separated program from its evaluation, the choicebits used for garbling the circuit have to be persisted.