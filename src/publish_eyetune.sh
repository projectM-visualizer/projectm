#!/bin/bash

# Options
repo=projectm
SRC=/d/GIT/$repo
DEST=$SRC/projectm
###
mkdir -p $DEST

# Define a timestamp function
timestamp() {
	date +"%Y%m%d%H%M%S"
}

# Move 1 file
move1() {
	cd $SRC/to_sync && find . -type f |head -1|xargs mv -t $DEST
}

# Upload 10 at a time
upload1() {
	cd $SRC && git pull
	cd $SRC && git status 
	cd $SRC && git add .
	cd $SRC && git commit -m"preset_mega_pack-cognitive_api-sync-$(timestamp)"
	cd $SRC && git push
	cd $SRC && git status
	sleep 55s # Waits 55 seconds.
}



# iterates
i="0"
while [ $i -lt 1000 ]
do
 echo "iteration number $i"
 i=$[$i+1]
 # $(upload1)
done

