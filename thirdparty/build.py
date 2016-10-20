#!/bin/sh
# -*- coding: utf-8 -*-

import os
import shutil

MODULE_PATH = os.path.dirname(os.path.abspath(__file__))

def safe_mkdir(path):
	if not os.path.exists(path):
		os.mkdir(path)

def call_cmd(cmd, quiet):
	if quiet: cmd += " > log.txt"
	os.system(cmd)

def main():
	quiet = True
	input_path = MODULE_PATH
	install_path = os.path.join(os.path.dirname(input_path), "dependency")

	build_path = os.path.join(MODULE_PATH, "build")
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

			cmd = '''cmake -DCMAKE_INSTALL_PREFIX="%s" "%s"''' % (install_path, source_path)
			call_cmd(cmd, quiet)
			call_cmd("make", quiet)
			call_cmd("make install", quiet)

	os.chdir(cwd)
	return

if __name__ == "__main__":
	main()
