# README : Getting Started

----
### Clone Git repository
First, clone the current repository :
 
`git clone git@github.com:cocobot/eurobot-base.git`

### Invoke the prepare script
This script will clone all the repositories you need including the `aussbee` fork :

`./prepare.sh`

### Build the project
First, navigate thrue the project you want to build, for ex here `pbrain` :

`cd projects/pbrain`

You might not have all the required packages, here's the list of the packages needed (cf. [`Ausbee` README](https://github.com/cocobot/ausbee/blob/master/README.md)) : 

* `gperf`
* `flex`
* `bison`
* `libncurses-dev` (for `menuconfig`)
* `wget`
* `unzip`
* `tar`
* `stm32flash`

Install them with the following commands :

`sudo apt-get update`
`sudo apt-get install gperf flex bison libncurses-dev wget unzip tar stm32flash`

Then configure the project :
`make menuconfig`

**Don't touch any settings**, exit and save configuration.

You can now build the project :

*  Build target : `make`
*  Build simulator : `make sim`

### You're done
