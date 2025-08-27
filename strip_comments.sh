#!/bin/bash

# This script removes comments from a range of files.
# Usage: ./strip_comments.sh <start_line> <end_line>

# Find all C/C++ source and header files
FILES=$(find src -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h")

START=$1
END=$2

# Get the lines from START to END
FILES_TO_PROCESS=$(echo "$FILES" | sed -n "${START},${END}p")

for FILE in $FILES_TO_PROCESS
do
  echo "Processing $FILE"
  # Use a temporary file to store the output
  TMPFILE=$(mktemp)
  # Use perl to remove comments.
  perl -0777 -p -e 's{/\*.*?\*/|//[^\n]*}{}gs' "$FILE" > "$TMPFILE"
  # Overwrite the original file with the comment-free version
  mv "$TMPFILE" "$FILE"
done

echo "Done processing batch $START-$END."
