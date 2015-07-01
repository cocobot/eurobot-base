#/bin/sh

#getting hardware
git clone git@github.com:cocobot/hardware.git


#getting aussbee fork
git clone -b cocobot git@github.com:cocobot/ausbee.git


#getting ausbee custom platforms
git clone git@github.com:cocobot/ausbee-platforms.git

#getting projects
mkdir projects

# - brain
git clone git@github.com:cocobot/pbrain.git projects/pbrain
