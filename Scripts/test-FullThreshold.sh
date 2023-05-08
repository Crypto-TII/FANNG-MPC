#!/bin/bash
# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/..

for test in test_OGC_LOADGC_EGC test_OGC_LOADGC_EGC_float test_gc_gen; do
	$ROOT/compile.sh -O1 Programs/$test || exit 1
  Scripts/run-online.sh Programs/$test || exit 1
done

