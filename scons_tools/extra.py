#!/usr/bin/env python

import os
import time
import SCons
import re
import string
import sys
import glob

from SCons.Script import *
from SCons.Script.SConscript import SConsEnvironment

import SCons.Script.SConscript
import SCons.SConf
import SCons.Conftest
from scons_tools.colors import *


def CheckPKG(context, name):
	context.Message(Colors.COMPILE + 'Checking for ' + Colors.TARGET + name + '... ' + Colors.END )
	ret = context.TryAction('pkg-config --exists "%s"' % name)[0]
	context.Result( ret )
	return ret
	
def ConfirmLibs(env, libs, headers):

	conf = Configure(env, custom_tests = { 'CheckPKG' : CheckPKG, 'CheckHeader' : CheckHeader})

	failed = False
	for evar, args in libs.iteritems():
		found = False
		for name, version in args['checks'].iteritems():
			if conf.CheckPKG(name + ' ' + version):
				env[evar] = name
				found = True;
				break
		if not found:
			if 'required' in args:
				if not args['required']==False:
					failed = True
			else:
				failed = True


	for evar, args in headers.iteritems():
		found = False
		for name, version in args['header'].iteritems():
			path = conf.CheckHeader(name, args['dirs'])
			if path:
				env[evar] = path
				found = True;
				break
		if not found:
			if 'required' in args:
				if not args['required']==False:
					failed = True
			else:
				failed = True
 

	if failed:
		exit(1)

	return conf.Finish()
	

def InstallPerm(env, dir, source, perm):
	obj = env.Install(dir, source)
	for i in obj:
		env.AddPostAction(i, Chmod(i, perm))
	return dir

SConsEnvironment.InstallPerm = InstallPerm

SConsEnvironment.InstallProgram = lambda env, dir, source: InstallPerm(env, dir, source, 0755)
SConsEnvironment.InstallData = lambda env, dir, source: InstallPerm(env, dir, source, 0644)


def MatchFiles (files, path, repath, dir_exclude_pattern,  file_exclude_pattern):

	for filename in os.listdir(path):
		fullname = os.path.join (path, filename)
		repath_file =  os.path.join (repath, filename);
		if os.path.isdir (fullname):
			if not dir_exclude_pattern.search(repath_file):
				MatchFiles (files, fullname, repath_file, dir_exclude_pattern, file_exclude_pattern)
		else:
			if not file_exclude_pattern.search(filename):
				files.append (fullname)

	
def GetSourceFiles(env, dir_exclude_pattern, file_exclude_pattern):
	dir_exclude_prog = re.compile(dir_exclude_pattern)
	file_exclude_prog = re.compile(file_exclude_pattern)
	files = []
	MatchFiles(files, env.GetLaunchDir(), os.sep, dir_exclude_prog, file_exclude_prog)
	return files
	
def GetVersionInfo(env):
	revision = os.popen('svnversion -n %s' % env.GetLaunchDir() ).read()
	revision=revision.replace(':','.')
	revision=revision.replace('P','')
	revision=revision.replace('S','')
	env.Replace(GPICK_BUILD_REVISION = revision,
		GPICK_BUILD_DATE =  time.strftime ("%Y-%m-%d"),
		GPICK_BUILD_TIME =  time.strftime ("%H:%M:%S"));		

def RegexEscape(str):
	return str.replace('\\', '\\\\')

def WriteNsisVersion(target, source, env):
	for t in target:
		for s in source:
			file = open(str(t),"w")
			file.writelines('!define VERSION "' + str(env['GPICK_BUILD_VERSION'])+'.'+str(env['GPICK_BUILD_REVISION']) + '"')
			file.close()
	return 0

def Glob(path):
	files = []
	for f in glob.glob(os.path.join(path, '*')):
		if os.path.isdir(str(f)):
			files.append(Glob(str(f)));
		else:
			files.append(str(f));
	return files


def CheckHeader(conf, header_name, check_paths):
	conf.Message(Colors.COMPILE + 'Checking for header ' + Colors.TARGET + header_name + '... ' + Colors.END)
	for path in check_paths:
		if os.path.isfile(os.path.join(path, header_name)):
			conf.Result('yes')
			return path
	conf.Result('no')
	return None

