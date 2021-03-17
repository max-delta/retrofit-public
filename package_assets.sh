#!/bin/bash

ROOT=`pwd`
OUTPUT="$ROOT/_packages"

mkdir -p "$OUTPUT"

tar \
	--create \
	--file="$OUTPUT/cc3o3.tar.xz" \
	--directory="$ROOT" 'create_scratch_directories.bat' \
	--directory="$ROOT" 'config/vfs_core.ini' \
	--directory="$ROOT" 'config/vfs_cc3o3.ini' \
	--directory="$ROOT" data \
	--directory="$ROOT/../retrofitassets" data \
	--directory="$ROOT/../retrofitassets/third_party" data \
	--directory="$ROOT/../rftest_data" data \
	--directory="$ROOT/../rftest_data/unlicensed" data \
	--xz
