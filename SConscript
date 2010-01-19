#!/usr/bin/env python

import os
import string
import sys

from scons_tools.extra import *
from scons_tools.environment import *

env = SMEnvironment(ENV=os.environ)

vars = Variables(os.path.join(env.GetLaunchDir(), 'user-config.py'))
vars.Add('DESTDIR', 'Directory to install under', '/usr/local')
vars.Add('DEBUG', 'Build with debug information', False)
vars.Update(env)

try:
	umask = os.umask(022)
except OSError:     # ignore on systems that don't support umask
	pass

if not env.GetOption('clean'):
	env = ConfirmLibs(env, {
		'GLIB_PC': 			{'checks':{'glib-2.0':'>= 2.16.0'}},
		'GDK_PC': 			{'checks':{'gdk-2.0':'>= 2.16.0'}},
		'LUA_PC': 			{'checks':{'lua':'>= 5.1', 'lua5.1':'>= 5.1'}},
		'GTHREAD_PC':		{'checks':{'gthread-2.0':'>= 2.22'}},
		})
		
if env['DEBUG']==True:
	env.Append(
		CPPFLAGS=['-Wall', '-g3', '-O0'], CFLAGS=['-Wall', '-g3', '-O0'],
		)
else:
	env.Append(
		CPPDEFINES=['NDEBUG'], CDEFINES=['NDEBUG'],
		CPPFLAGS=['-Wall', '-O2'], CFLAGS=['-Wall', '-O2'],
		)


	
env['LIBPATH']=['.']

env.Replace(
	SHCCCOMSTR = "Compiling ==> $TARGET",
	SHCXXCOMSTR = "Compiling ==> $TARGET",
	CCCOMSTR = "Compiling ==> $TARGET",
	CXXCOMSTR = "Compiling ==> $TARGET",
	SHLINKCOMSTR = "Linking shared ==> $TARGET",
	LINKCOMSTR = "Linking ==> $TARGET",
	LDMODULECOMSTR = "Linking module ==> $TARGET",
	ARCOMSTR = "Linking static ==> $TARGET"
	)

Decider('MD5-timestamp')

extern_libs = SConscript(['extern/SConscript'], exports='env')
executable, sysmony_core, render_engine = SConscript(['src/SConscript'], exports='env')


env.Alias(target="build", source=[
	executable,
	sysmony_core,
	render_engine,
])

env.Alias(target="install", source=[
	env.InstallProgram(dir=env['DESTDIR'] +'/bin', source=[executable]),
	env.InstallProgram(dir=env['DESTDIR'] +'/lib', source=[sysmony_core, render_engine]),
	
	env.InstallData(dir=env['DESTDIR'] +'/share/sysmony', source=[env.Glob('share/sysmony/*.lua')]),
	env.InstallData(dir=env['DESTDIR'] +'/share/sysmony/logo', source=[env.Glob('share/sysmony/logo/*.png')]),
	
])


env.Default("build")


