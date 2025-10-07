#!/bin/bash

# If the user kills this script early we want to make sure that all
# running instances of cbp3 are killed
trap "echo; echo 'Killing all running cbp3 processes'; killall -e cbp3; exit 1;" SIGINT

# Make the simulation executable
echo "--- Building the project ---"
make 

# Check that the build command succeeded
if [[ $? -eq 0 ]]; then
  echo "--- Finished building the project ---"
  echo
else
  echo "Build failed!"
  exit 1
fi

# Check for the "traces" directory
if [ ! -d "traces" ]; then
  echo
  echo "Error: Directory 'traces/' not found in the current directory."
  echo
  echo "The traces are located on the canvas page and should be untarred into the"
  echo "$(pwd)/traces/ directory."
  exit 1
fi

# Check for the "cbp3" executable
if [ ! -x "./build/cbp3" ]; then
  echo
  echo "Error: Executable 'cbp3' not found or not executable in the ./build/ directory."
  exit 1
fi

# Check for the "runs" directory
if [ ! -d "runs" ]; then
  echo
  echo "Warning: Directory 'runs/' not found in the current directory."
  echo "         Creating 'runs/' directory..."
fi

# Run the 16 traces in parallel (denoted by the '&' at the end of the command)
echo "Starting simulation on all 16 trace simulations"
for i in {1..16}
do
    p=$(printf "%02d" $i)
    ./build/cbp3 -t traces/CLIENT${p}.bz2 > runs/output_file_${i}.txt &
done

# Wait for each of the traces to finish running (this may take a while)
echo "Waiting for all simulation runs to finish..."
echo "(This may take some time to finish)"
wait

# Calculate the score from the autograding script
score=$(./generateScore.pl --from runs/ | grep -E "Total performance score:" | sed -e 's/Total performance score: \(.*\).*/\1/g')

# Print the score 
echo "Scored ${score}"

# Ensure that all cbp3 processes are no longer running
killall -e cbp3 &> /dev/null
