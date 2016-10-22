#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import shutil
from argparse import ArgumentParser

MODULE_PATH = os.path.dirname(os.path.abspath(__file__))

MAKE = "nmake" if sys.platform == "win32" else "make"
GENERATOR = "NMake Makefiles" if sys.platform == "win32" else "Unix Makefiles"

def safe_mkdir(path):
	if not os.path.exists(path):
		os.mkdir(path)

def call_cmd(cmd, quiet):
	if quiet: cmd += " > log.txt"
	else: print cmd

	os.system(cmd)

def main():
	parser = ArgumentParser(description = "build third party tools")
	parser.add_argument("--verbose", action="store_true", help="show more log")
	parser.add_argument("--rebuild", action="store_true", help="rebuild all projects.")

	option = parser.parse_args()
	quiet = not option.verbose

	input_path = MODULE_PATH
	install_path = os.path.join(os.path.dirname(input_path), "dependency")
	build_path = os.path.join(MODULE_PATH, "build")

	if option.rebuild and os.path.exists(build_path):
		shutil.rmtree(build_path)

	safe_mkdir(build_path)

	cwd = os.getcwd()

	files = os.listdir(input_path)
	for path in files:
		source_path = os.path.join(input_path, path)
		if os.path.isdir(source_path) and os.path.exists(os.path.join(source_path, "CMakeLists.txt")):
			print "compile:", source_path

			temp_path = os.path.join(build_path, path)
			safe_mkdir(temp_path)

			os.chdir(temp_path)

			cmd = '''cmake -G"%s" -DCMAKE_INSTALL_PREFIX="%s" "%s"''' % (GENERATOR, install_path, source_path)
			call_cmd(cmd, quiet)
			call_cmd("%s" % (MAKE, ), quiet)
			call_cmd("%s install" % (MAKE, ), quiet)

	os.chdir(cwd)
	return

if __name__ == "__main__":
	main()
