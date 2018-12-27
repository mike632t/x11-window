#
#	x11-window.make
#
#	Copyright (C) 2013  MEJT
#	
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#	
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#	
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#	Note seperator (tab) at the beginning of the line CANNOT be a space..!
#	
#	01 May 13   0.1	- Initial version derived from gtk-sample-menu - MEJT
#  26 Dec 18 			- Tidies up makefile and added make clean option - MEJT
#
PROGRAM = x11-window
SOURCE = $(PROGRAM).c
OBJECT = $(SOURCE:.c=.o)
OUTPUT = $(PROGRAM).out
FLAGS	= 
LIBS	= -lX11
CC	= gcc
#
$(PROGRAM): $(OBJECT) 
#	@echo '*** Linking $(OBJECT)'
	@$(CC) $(FLAGS) $(OBJECT) -o $(PROGRAM) $(LIBS)
#	Tidy up by deleting ALL object files - this will force all the sources
#	to be recompiled every time so may not be a good idea on slow machines
#	@echo '*** Deleting object files'
	@rm -f $(OBJECT)
#
#  Doesn't attempt to be selective - just recompiles everything!
$(OBJECT) : $(SOURCE)
#	@echo '*** Compiling $(SOURCE)'
	@$(CC) $(FLAGS) -c $(SOURCE)
#
#  make clean - Clean up any object files.
clean:
#	@echo '*** Cleaning up object files'
	@rm -f $(OBJECT)
