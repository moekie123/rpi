#!/usr/bin/bash

OUTPUT=~/.bash_aliases

if [ ! -d .generated ]; then
	mkdir .generated
fi

echo "PROJECT_DIR=$PWD" > .generated/.bash_aliases
cat script/aliases >> .generated/.bash_aliases
cp .generated/.bash_aliases $OUTPUT

source ~/.bashrc

echo "development shortcuts added in ~/.bash_aliases"
