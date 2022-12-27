#!/bin/bash
set -e

ROOT=`dirname -- "$(readlink -f -- "$0";);"`

FILES=`find \
	$ROOT/apps \
	$ROOT/libs \
	-name "*.vcxproj" -or \
	-name "*.vcxproj.filters"`

for FILE in $FILES; do
	echo "FILE: $FILE"

	# Convert /r/n -> /n
	sed --expression='s/\r$//g' --in-place "$FILE"

	# Gross hack to assume XML has two-space tabs, and fix them to be actual tabs
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
	sed --expression='s/^\(\t*\)  /\1\t/g' --in-place "$FILE"
done
