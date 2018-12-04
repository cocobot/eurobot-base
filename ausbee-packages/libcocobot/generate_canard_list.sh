#!/bin/sh

python3 ../libcanard/libcanard/dsdl_compiler/libcanard_dsdlc --outdir src/com/dsdl/ dsdl/uavcan/
python3 ../libcanard/rust_dsdl/canars_dsdl --outdir ../../projects/cocoui/dsdl/src/lib.rs dsdl/uavcan/ -v
