#!/usr/bin/env python

import os
import string
import sys

from scons_tools.extra import *
from scons_tools.environment import *
from scons_tools.colors import *

env = SMEnvironment(ENV=os.environ)

variables = Variables(os.path.join(env.GetLaunchDir(), 'user-config.py'))
variables.Add('DESTDIR', 'Directory to install under', '/usr/local')
variables.Add('DEBUG', 'Build with debug information', False)
variables.Update(env)

try:
	umask = os.umask(022)
except OSError:     # ignore on systems that don't support umask
	pass

if not env.GetOption('clean'):
	env = ConfirmLibs(env, {
		'GLIB_PC': 			{'checks':{'glib-2.0':'>= 2.16.0'}},
		'DBUSGLIB_PC':		{'checks':{'dbus-glib-1':'>= 0.76'}},
		'GDK_PC': 			{'checks':{'gdk-2.0':'>= 2.16.0'}},
		'LUA_PC': 			{'checks':{'lua':'>= 5.1', 'lua5.1':'>= 5.1'}},
		'GTHREAD_PC':		{'checks':{'gthread-2.0':'>= 2.22'}},
		},{
		#'EIGEN_INCLUDE_PATH':	 {'header':{'Eigen/Core':''}, 'dirs':['/usr/include/eigen2', '/usr/local/inlude/eigen2']},
			
			
		})

if env['DEBUG']:
	env.Append(
		CPPFLAGS=['-Wall', '-g3', '-O0'], CFLAGS=['-Wall', '-g3', '-O0'],
		)
else:
	env.Append(
		CPPDEFINES=['NDEBUG'], CDEFINES=['NDEBUG'],
		CPPFLAGS=['-Wall', '-O3'], CFLAGS=['-Wall', '-O3'],
		)

#env.Append(CPPPATH = env['EIGEN_INCLUDE_PATH'])
	
env['LIBPATH']=['.']

env.Replace(
	SHCCCOMSTR = Colors.COMPILE + "Compiling ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	SHCXXCOMSTR = Colors.COMPILE + "Compiling ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	CCCOMSTR = Colors.COMPILE + "Compiling ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	CXXCOMSTR = Colors.COMPILE + "Compiling ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	SHLINKCOMSTR = Colors.LINK + "Linking shared ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	LINKCOMSTR = Colors.LINK + "Linking ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	LDMODULECOMSTR = Colors.LINK + "Linking module ==> " + Colors.TARGET + "$TARGET" + Colors.END,
	ARCOMSTR = Colors.LINK + "Linking static ==> " + Colors.TARGET + "$TARGET" + Colors.END
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


