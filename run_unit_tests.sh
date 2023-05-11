#!/bin/bash
make -B test
./Test/Test-Serialization.x
./Test/Test-Choicebits.x
./Test/Test-GC_Storage.x
