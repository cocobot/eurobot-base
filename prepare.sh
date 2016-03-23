#/bin/sh

#getting hardware
git clone git@github.com:cocobot/hardware.git


#getting aussbee fork
git clone git@github.com:cocobot/ausbee.git
#use branch until changes are merged into master
cd ausbee
git checkout dev_host_build
cd ..


#getting ausbee custom platforms
git clone git@github.com:cocobot/ausbee-platforms.git

#getting projects
mkdir projects

# - brain loader
git clone git@github.com:cocobot/brain_loader.git projects/brain_loader

# - brain (main robot)
git clone git@github.com:cocobot/pbrain.git projects/pbrain

# - brain (secondary robot)
git clone git@github.com:cocobot/sbrain.git projects/sbrain

# - cocoui
git clone git@github.com:cocobot/cocoui.git projects/cocoui

# - cocosim
git clone git@github.com:shazame/cocosim.git projects/cocosim
