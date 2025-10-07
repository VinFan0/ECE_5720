# Branch Prediction Lab

## Assignment Instructions

The canvas page has detailed instructions on how to get started on this code. If you've already
read those instructions, here is a quick refresher:

- You should only make changes to [predictor.cc](./predictor.cc) and [predictor.h](./predictor.h).

- You need to implement the three branch predictors (2-bit saturating, gshare, gselect) in
    predictor.cc.

## Build Instructions

This project uses a Makefile and CMake to compile the code. It should compile on most Linux
systems with a compiler that supports `c++11` (or newer) and a system with Cmake 2.8 or
newer.

To build the code just run make from this directory:

```shell
make
```

To remove all build files run:

```shell
make clean
```

## Running Instructions

### Single Trace Run

After building your code, run it by calling the `cbp3` executable located in the build directory.
If you didn't edit anything in the Makefile or CMakeLists file, the build directory will be
simply named `build/`.

```shell
./build/cbp3; # runs cbp3 with no arguments
```

To run `cbp3` with a trace file, pass the file using the `-t` argument. Make sure to redirect the
output to a file unless you want the output to show up on `stdout`.

```shell
./build/cbp3 -t ./<path_to_zipped_trace_file> > runs/<output_file_name>.txt
```

### Multiple Trace Run

A bash script [run\_traces.sh](./run_traces.sh), has been included in this project that
automatically runs `cbp3` on all of the traces, as long as the traces were unzipped in
the same directory as predictor.cc and the Makefile.

### Grading

To run the autograder first run all traces using the previously mentioned bash script,
the run the autograder perl script:

```shell
./generateScore.pl --from runs/
```
