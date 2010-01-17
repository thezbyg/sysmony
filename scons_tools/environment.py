#!/usr/bin/env python

import os
import time
import SCons
import re
import string
import sys
import glob
import subprocess

from SCons.Script import *
from SCons.Util import *
from SCons.Script.SConscript import SConsEnvironment

import SCons.Script.SConscript
import SCons.SConf
import SCons.Conftest

class SMLibrary(NodeList):
	include_dirs = []

class SMEnvironment(SConsEnvironment):
	
	extern_libs = {}

	def DefineLibrary(self, library_name, library):
		self.extern_libs[library_name] = library
		
	def UseLibrary(self, library_name):
		lib = self.extern_libs[library_name]
		
		for i in lib:
			lib_include_path = os.path.split(i.path)[0]
			self.AppendUnique(LIBS = [library_name], LIBPATH = ['#' + lib_include_path])
			
		self.AppendUnique(CPPPATH = lib.include_dirs)
		
		return lib
