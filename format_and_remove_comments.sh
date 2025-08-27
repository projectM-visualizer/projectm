#!/bin/bash

# Find all C/C++ source and header files
FILES=$(find src -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h")

for FILE in $FILES
do
  echo "Processing $FILE"
  # Run clang-format in-place
  clang-format -i "$FILE"
done

echo "Done."
