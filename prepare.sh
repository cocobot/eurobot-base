#/bin/sh

#getting hardware
git clone git@github.com:cocobot/hardware.git


#getting aussbee fork
git clone git@github.com:cocobot/ausbee.git
#use branch until changes are merged into master
cd ausbee
git checkout dev_host_build
cd ..

#pulling all submodules
git pull && git submodule sync --recursive && git submodule update --init --recursive
