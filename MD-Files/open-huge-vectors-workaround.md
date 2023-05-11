# Huge vectors `POpen_Start` and `POpen_Stop` workaround

## Introduction

When we try to open a vector with many elements (the upper limit depends on the configuration but we are using vectors 
of size 100,000 as limit) the execution of scale/mamba never ends.

## Solution
New versions of `POpen_Start` and `POpen_Stop` called `POpen_Start_Batched` and `POpen_Stop_Batched` have been created.
These versions are used when we try to open vectors larger than 100,000 elements. The way they work is by 
creating more connections and splitting the vector into batches of size 100,000 before send them though openssl. 
The batch size and maximum number of connections can be configured in [`config.h`](../src/config.h) file.
If there are fewer connections than the number we need to open a vector, a `Not_enough_ssl_connections_exception` 
exception will be thrown.

It is important to know that the first three connections are reserved to be used by Scale Mamba in the usual way, 
so for this solution, the first used connection will be connection 3.

### Note
When the vector size is less than 100,000 we still use the old version of `POpen_Start` and `POpen_Stop` to maintain 
compatibility and avoid possible unknown bugs related to the workaround.