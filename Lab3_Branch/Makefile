# Author: Nick Waddoups 
# Date: Oct 19, 2024
#
# Created for ECE 5720 - Computer Architecture - Branch Prediction Lab
#
# This will be the same as the makefile used by the grader, so only
# edit it if you are sure that the grader will still be able to
# compile it

# Sets the build directory
BUILD_DIR := build

# Builds the code using CMake and places build files in the build directory
.PHONY : build
build: builddir
	cmake --build ${BUILD_DIR} 
	@echo
	@echo "Built binaries, they are located in $(shell pwd)/${BUILD_DIR}/"

# Creates the build directory, it's okay if it already exists
.PHONY : builddir
builddir:
	@mkdir -p build
	@cd build; cmake ..

# Removes all build files
.PHONY : clean
clean :
	rm -r ${BUILD_DIR}

# Run cbp3 on the test trace that comes with the project
.PHONY : test
test: build
	@echo $(shell ./build/cbp3 -t trace_test.bz2 > runs/test_output.txt)
	@echo "Ran build on trace_test.bz2, output is in the runs/ directory"
