#!/bin/bash

set -ex

# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/..

run_test() {
    test=$1
    optimizations=$2
    shift
    echo "$test"
    printf "\n\n\n\n\n\n\n\n\n\n"
    $ROOT/compile-rust.sh $optimizations $test
    if [ "$NO_RUN" != "1" ]; then
        if [ "$EMULATE_ONLY" != "1" ]; then
            Scripts/run-online.sh $ROOT/RustPrograms/examples/$test
        fi
        env RUSTC_BOOTSTRAP=1 SHARING_DATA=$ROOT/Data/SharingData.txt cargo run --manifest-path=$ROOT/RustPrograms/Cargo.toml --features="emulate" --example $test
    fi
}

if test "$1"; then
    test=$1
    shift
    run_test $test $*
else
    for test in test_call test_recursion; do
        OPTS= run_test $test
    done
    # Deliberately not calling test_recursion and test_call here
    # Do not call any routines here which requires a big prime
    for test in test_array test_array_arith test_bitdecfull test_GC test_ieee test_modp test_local_functions test_stacks; do
	run_test $test
    done
    printf "\n\n\n\n\n\n\n\n\n\n"
fi

printf "All tests ran successfully"
