# What is this?

This repository contains the source code for the Play-by-Mail strategy game [Eressea](http://www.eressea.de/).

# Prerequisites

Eressea depends on a number of external libraries. On a recent Debian-based Linux system, this is the apt-get command to install all of them.

    sudo apt-get install git cmake gcc make libxml2-dev liblua5.2-dev libtolua-dev libncurses5-dev libsqlite3-dev

# Server Setup

These instructions are for an Ubuntu Linux system.

# Directory Structure

It is recommended creating a directory in your Home folder called eressea and installing all files inside that directory. However, you can use any directory and change all the later references of the eressea directory to the directory you created.

	cd
	mkdir eressea && cd eressea

# Downloading of files

You do not need the rules or echeck files to run the game, but the files are recommended when running a game for rules reference and checking orders files. The server files will be downloaded into directory git. You can clone the repositories to your own github account or you can just clone the official eressea repositories to get you started.

	git clone --recursive git://github.com/eressea/server.git git
	git clone --recursive git://github.com/eressea/rules.git
	git clone --recursive git://github.com/eressea/echeck.git

# Configuration of files

This repository relies heavily on the use of submodules, and it pulls in most of the code from those. The build system being used is cmake, which can create Makefiles on Unix, or Visual Studio project files on Windows. Here's how you build the source on Ubuntu. You should already be in the eressea directory.

    cd git
    ./configure
    s/build && s/runtests && s/install

If you got this far and all went well, you have built and installed a server, and it will have passed some basic functionality tests.

# Create a game

From inside the git directory run the following code to create your first game. The number after -g is the game number you choose and the ruleset is chosen after -r. Current eressea rulesets are e2, e3, or e4.

	ERESSEA=$HOME/eressea s/setup -g 1 -n -r e2
	
This creates a directory inside the eressea directory named game-1 with 1 being the game number you designated above. You now have a completed server and game files. You still must create a data world before the game can be run.

# Data world creation

You can view sample scripts for world creation in eressea/server/scripts/tools/build-e{3,4}.lua. These are not ready to use but will be good reference material on how to create your own script. Alternatively the long hand example below will allow you to manually create a data file.
	
Navigate to the eressea game-1 directory and run:

	./eressea
			
This will put you at an eressea command line E>
You can now run the following commands to create a quick test world.

	dofile('config.lua')
	-- clear the game data
	eressea.free_game()
	-- make the world 11*11 hexes, and wrap at the edges
    -- with coordinates in [-5..5],[-5..5]
	pl = plane.create(0, -5, -5, 11, 11)
	-- create a new region
	r = region.create(0, 0, "plain")
	-- ... ditto for "ocean", "mountain", "highland", etc.
	-- create a new player faction (humans, english language)
	f = faction.create("email@email.com", "human", "en")
	-- create a 1-person unit for this faction, in that region
	u = unit.create(f, r, 1)
	-- give it some stuff
	u:add_item("money", 5000)
	u:add_item("horse", 1)
	u:add_item("log", 5)
	-- write the game to disk
	eressea.write_game("0.dat")
	-- export the world to JSON (map only)
	eressea.export("map.json", 1)
	-- export the world to JSON (with factions and units)
	eressea.export("world.json", 7)
	
Alternatively if you pasted this above code into a file and named it build.lua and ran it by calling ./eressea build.lua from the game-1 directory it would automatically create a quick test world.

The export function at the end of the example script hints at another way to create the world. If you have another mapping tool, and you can convert its output to the same JSON format, then you could create a world by simply importing it from that interchange format.

If you created “world.json” outside of e> then you need to import and write the the game file

    eressea.import("world.json")
	
Whichever process you created “world.json” in you now need to write the data to eressea

    eressea.write_game("0.dat")

The game does have a map editor which can now be used instead of scripting. You must still be at an eressea command line E> or run ./eressea again from eressea/game-1.

	E> dofile('config.lua')
	E> eressea.read_game("0.dat")
	E> gmtool.editor()

This reads the configuration data, then reads the tiny world we created earlier and starts the editor. It will look as if there is more than one plain on the screen. Don't let that fool you because the world wraps at the edges, so this is all the same region. You can navigate the world with the arrow keys, and there are a lot of other keyboard combinations that are poorly documented. Try Ctrl-T for terraforming, Space to select/deselect a region, and semicolon followed by t to terraform the current selection. It's a little clunky, but it allows interactive map editing. Shift-S saves the game.

# Game config

Each game has slightly different rules and needs to know where the configuration data lives, so the game directory contains an eressea.ini file. The file looks like this:

	[eressea]
	report		= reports
	locales		= de,en
	[lua]
	install		= /home/eressea/eressea/server
	paths		= /home/eressea/eressea/server/scripts:/home/eressea/eressea/server/lunit
	rules		= e2
	kill_after	= 3
	maxnmrs	= 5

# Initial reports and running turns

After your game world has been successfully created you can have the server generate your first reports. They will be placed in the reports directory for that game. From the game-1 directory run:

	./eressea -t 0 reports.lua
	
You need to place all orders in a file named orders.0 where 0 represents the turn number. The orders files should be placed inside the game-1 directory. After you have all orders in your orders file you run a turn from the game-1 directory with:

	./eressea -t 0 run-turn.lua
	
The number after the -t represents the turn you wish to run. The first turn will always be 0 and all other turns will be sequential starting at 2. After successfully running a turn, the server will automatically generate the reports for that turn and place them in the reports directory.

You should now have a fully functional game. Congrats!