
# Input output system
The default input/output system is the database input/output system (`input_output_db.cpp`). 

Check how to set the databases up in the section [_Input/Output database system_](#inputoutput-database-system--mysql-) below.

Check how to change between the available Input/Output systems in the section [_Switching between the available Input/Output systems_](#switching-between-the-available-inputoutput-systems) below.

## Important convention about Input/Output
When loading `fix` data, floating notation must be used, i.e. `1.0` and the use of any other format will
lead to unexpected behaviors. On the other hand, when loading `int` data, the I/O system can be used as before.

## Input/Output filesystem

A new Input/Output system based on the use of regular files for input and output has been developed.
The channels are unidirectional, so a channel can only be used as either input or output. In order to perform the opposite operation over an existing channel, first it would have to be closed and then opened again with the correct operation type.

Whether a channel is going to be used for input or output is defined when the channel is opened, as well as the name of the file that will be associated to it. The input and output files are stored in `Data` folder by default.

The default channel, channel 0, won't use filesystem input/output because it is used for **debugging** purposes. This
channel will still be bidirectional and use the standard input/output system.

The program `IO_example` contains different examples.

### Non-interactive use of the Input/Output filesystem

There are two options to run programs that require the use of the I/O filesystem **non-interactively**.
We say a program runs _non-interactively_ when it does not wait for user input because it is provided beforehand.


#### For testing purposes

Using the `run-online-interactive-with-answers.sh` script.
It requires two arguments: a string with all the responses **in the order they are required by the test program** and
the program's path. `test_matrix_lib` could be executed non-interactively running:

```./Scripts/run-online-interactive-with-answers.sh "n out.txt y out.txt" Programs/test_matrix_lib```

The string `"o out.txt i out.txt"` contains the whitespace separated answers to the 4 questions that `Programs/test_matrix_lib` will ask:

1. `Will channel 9999 be used for input? (i/o)` → o
2. `Introduce the filepath to use as output:` → out.txt
3. `Will channel 9999 be used for input? → (i/o)` → i
4. `Introduce the filepath to use as output:` → out.txt

#### Running each player in separate terminals:

If you need a program to be executed by each player independently and in a non-interactive way, use

```echo "$player_answers" | ./Player.x $player_number Programs/test_matrix_lib &```

This command is very similar to a regular SM program execution, only with an extra `$player_answers` variable.
This variable would be the string with all the ordered responses needed to the run program, just as specified in the previous section.

## Input/Output database system (MySQL)
Each party requires a MySQL database to connect to. They can be different databases or the same one, that is up to you.  
Read further on this in [Storage Config File](./storage-config-file.md) and [Database Setup](./database-setup.md).

The Input/Output database system can be used non-interactively just like the Input/Output filesystem. 
Please note that, when using databases, the program will not ask for channel filepath, so update your player answers strings accordingly.

### Why is the party number a required argument for I/O database functions?
In order to allow reading a set of shares when using the input-output database system, player number is required for running the tests. This allows filtering the information that should be read by each player:

During the tests execution, every player (all of them launched from a single shell) will be inserting data into the test database and will be performing reading operations from it as well. In order distinguish which share belongs to each player, the player number [0-N] is also stored along with the share value. This way the shares can be filtered by the player number.

## Switching between the available Input/Output systems

The Input/Output database system is the default Input/Output system. The way to switch between the available Input/Output systems requires the user to modify `src/player.cpp` file and compile FANNG-MPC from scratch. 

Replace the lines of code in the snippets below to switch between Input/Output database and Input/Output filesystem in `src/player.cpp`:

_Input/Output database setup:_

```c++
    auto ios= std::make_unique<Input_Output_DB>();
    ios->init(my_number,true);
```


_Input/Output filesystem setup:_
```c++
    auto ios= std::make_unique<Input_Output_FS>();
    ios->init(true);
```
**NOTE**: We sometimes would like to be able to load decimal numbers (rational). If that is the case the compiler needs to know the precision that you might be using before hand. To that end we have included the following parameter in [config.h](/src/config.h):
```c++
    /* Defines SCALE precision when reading fixed point
    * mantisas from files and DB's.  
    */
    #define fixed_point_reading_precision 12
```
**NOTE**: The Input/Output system only uses the StorageConfig files to read the properties related to database connection.
The StorageConfig file also has filesystem related information but it is only used by the Offline Garbling subsystem.
Read further on this in [Storage Config File](./storage-config-file.md) and [Database Setup](./database-setup.md). 
When using Input_Output_FS, the necessary input or output files must be located inside Data folder. 