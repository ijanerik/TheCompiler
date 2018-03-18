# ---------------------------------------------------------------------------
#
# SAC Compiler Construction Framework
#
# ---------------------------------------------------------------------------
#
# SAC COPYRIGHT NOTICE, LICENSE, AND DISCLAIMER
#
# (c) Copyright 1994 - 2010 by
#
#   SAC Development Team
#   SAC Research Foundation
#
#   http://www.sac-home.org
#   email:info@sac-home.org
#
#   All rights reserved
#
# ---------------------------------------------------------------------------
#
# The SAC compiler construction framework, all accompanying
# software and documentation (in the following named this software)
# is developed by the SAC Development Team (in the following named
# the developer) which reserves all rights on this software.
#
# Permission to use this software is hereby granted free of charge
# exclusively for the duration and purpose of the course
#   "Compilers and Operating Systems"
# of the MSc programme Grid Computing at the University of Amsterdam.
# Redistribution of the software or any parts thereof as well as any
# alteration  of the software or any parts thereof other than those
# required to use the compiler construction framework for the purpose
# of the above mentioned course are not permitted.
#
# ---------------------------------------------------------------------------
#



###############################################################################
#
# Makefile for sac2c source directories
#
# This Makefile is copied on demand into source directories.
# Be sure not to edit these copies.
#
###############################################################################



###############################################################################
#
# General settings
#

PROJECT_ROOT = ../..

include $(PROJECT_ROOT)/Makefile.Config

TARGET = $(notdir $(PWD))



###############################################################################
#
# Dummy rules
#

.PHONY: clean all devel prod 

###############################################################################
#
# Start rules
#

all: devel

devel:
	@$(ECHO) ""
	@$(ECHO) "Making current subdirectory (developer version)"
	@$(MAKE) -C $(PROJECT_ROOT) TARGET="$(TARGET)" MODE="" CHECK_DEPS="yes" makesubdir

prod:
	@$(ECHO) ""
	@$(ECHO) "Making current subdirectory (product version)"
	@$(MAKE) -C $(PROJECT_ROOT) TARGET="$(TARGET)" MODE="prod" CHECK_DEPS="yes"makesubdir




###############################################################################
#
# Rules for generating Makefiles
#

Makefile: $(PROJECT_ROOT)/Makefile.Source
	@$(ECHO) "  Creating makefile: $@"
	@cp -f $< $@





###############################################################################
#
# Rules for directory cleaning
#

clean:
	@$(RM) *.o *.a *.bak *~ .*.d
	@$(RM) $(patsubst %.xsl,%,$(wildcard *.xsl))
	@$(RM) -r .sb SunWS_cache
	@$(RM) *.lex.c *.tab.c *.tab.h y.output
