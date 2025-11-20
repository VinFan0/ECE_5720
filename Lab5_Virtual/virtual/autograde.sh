# this is an example autograder to help in testing your address translation program
# the assignment will be graded by a similar script with different examples
# this script will leave the current directory unchanged

if [ -e constants.h ]; then mv constants.h constants_old.h; fi;
if [ -e constants.cpp ]; then mv constants.cpp constants_old.cpp; fi;
if [ -e virtual ]; then mv virtual virtual_old; fi;

########################################
# Examples from the book (two problems)
########################################

cp example1/constants.h .
cp example1/constants.cpp .

make

# test example from 9.6.4 in the book pg 823
./virtual 03d4 > output.txt
if grep -q 354 output.txt; then
  if grep -q HIT output.txt  && [ -z $(grep -q MISS output.txt) ]; then 
    echo "PASSED EXAMPLE 1"
  else
    echo "FAILED EXAMPLE 1: NO TLB HIT"
  fi
else
  echo "FAILED EXAMPLE 1: WRONG PHYSICAL ADDRESS"
  echo "  Virtual Addr: 0x03d4 Physical expected: 0x354 actual: "
  cat output.txt
fi

# test example from practice problem 9.4 in the book
./virtual 3d7 > output.txt
if grep -q 357 output.txt; then
  if grep -q HIT output.txt  && [ -z $(grep -q MISS output.txt) ]; then 
    echo "PASSED EXAMPLE 2"
  else
    echo "FAILED EXAMPLE 2: NO TLB HIT"
  fi
else
  echo "FAILED EXAMPLE 2: WRONG PHYSICAL ADDRESS"
  echo "  Virtual Addr: 0x3d7 Physical expected: 0x357 actual: "
  cat output.txt
fi

# test example from end of chapter problem 9.11
./virtual 27c > output.txt
if grep -q 5fc output.txt; then
  if grep -q MISS output.txt  && [ -z $(grep -q HIT output.txt) ]; then 
    echo "PASSED EXAMPLE 3"
  else
    echo "FAILED EXAMPLE 3: NO TLB MISS"
  fi
else
  echo "FAILED EXAMPLE 3: WRONG PHYSICAL ADDRESS"
  echo "  Virtual Addr: 0x27c Physical expected: 0x5fc actual: "
  cat output.txt
fi

# test example from end of chapter problem 9.12
./virtual 3a9 > output.txt
if grep -q 469 output.txt; then
  if grep -q MISS output.txt  && [ -z $(grep -q HIT output.txt) ]; then 
    echo "PASSED EXAMPLE 4"
  else
    echo "FAILED EXAMPLE 4: NO TLB MISS"
  fi
else
  echo "FAILED EXAMPLE 4: WRONG PHYSICAL ADDRESS"
  echo "  Virtual Addr: 0x3a9 Physical expected: 0x469 actual: "
  cat output.txt

fi

# test example from end of chapter problem 9.13
./virtual 40 > output.txt
if grep -q FAULT output.txt; then
  echo "PASSED EXAMPLE 5"
else
  echo "FAILED EXAMPLE 5: NO PAGE FAULT actual output:"
  cat output.txt
fi



########################################
# A custom example
########################################

cp example2/constants.h .
cp example2/constants.cpp .

make

./virtual 3D233 > output.txt

# A custom example with different sizes
if grep -q 6633 output.txt; then
  if grep -q HIT output.txt  && [ -z $(grep -q MISS output.txt) ]; then 
    echo "PASSED EXAMPLE 6"
  else
    echo "FAILED EXAMPLE 6: NO TLB HIT"
  fi
else
  echo "FAILED EXAMPLE 6: WRONG PHYSICAL ADDRESS"
  echo "  (using the constants in example2/ ) "
  echo "  Virtual Addr: 0x3D233 Physical expected: 0x6633 actual:"
  cat output.txt
fi


rm output.txt
if [ -e constants_old.h ]; then mv constants_old.h constants.h; fi;
if [ -e constants_old.cpp ]; then mv constants_old.cpp constants.cpp; fi;
if [ -e virtual_old ]; then mv virtual_old virtual; fi;

