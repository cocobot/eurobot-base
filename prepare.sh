#/bin/sh

#getting hardware
git clone git@github.com:cocobot/hardware.git


#getting aussbee fork
git clone git@github.com:cocobot/ausbee.git


#getting ausbee custom platforms
git clone git@github.com:cocobot/ausbee-platforms.git

#getting projects
mkdir projects

# - brain
git clone git@github.com:cocobot/pbrain.git projects/pbrain

# - cocoui
git clone git@github.com:cocobot/cocoui.git projects/cocoui

# - cocosim
git clone git@github.com:shazame/cocosim.git projects/cocosim
