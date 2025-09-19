"""
Helper script for remembering common build commands in my personal development on this repo.
"""

import os
import sys
import subprocess
import traceback

cmds = dict()
projaliases = {
	'fox': 'Heltec_v3_user_cparen_foxradio',
	'comp': 'Heltec_v3_companion_radio_ble'
}

def main():
	try:
		if len(sys.argv) == 1 or sys.argv[1] == '-h':
			show_usage();
		else:
			do_command(sys.argv[1], *sys.argv[2:])
	except Exception:
		traceback.print_exc()
		sys.exit(1)

def anno_command(fn):
	cmds[fn.__name__] = fn
	return fn

def show_usage():
	print("Usage: alias.py [command] [args]")
	print(f"  available commands: {(','.join(sorted(cmds.keys())))}")
	print(f"  use 'wh [command]' for more information")

def shell(command_string):
	print(f"executing {command_string}")
	ec = os.system(command_string)
	if ec: raise f"failed with exit code {ec}"

def do_command(command, *argv):
	if command not in cmds:
		raise "command not found"
	cmds[command](*argv)

@anno_command
def wh(command):
	"(wh)at: Show docstring for the command"
	if command not in cmds:
		raise "command not found"
	print(cmds[command].__doc__)

@anno_command
def pa():
	"Print available project aliases"
	print("available projects to build or flash")
	for k,v in projaliases:
		print(f"{k}: {v}")

@anno_command
def build(project_name):
	"Build a project firmware. Use 'pa' to see list of project aliases."
	target = projaliases[project_name]
	shell(f"./build.sh build-firmware {target}")

@anno_command
def flash(project_name):
	"Flash a resulting firmware. Use 'pa' to see list of project aliases."
	target = projaliases[project_name]
	commit =  subprocess.check_output("git rev-parse --short HEAD", shell=True, text=True).strip()
	fwver = os.environ["FIRMWARE_VERSION"]
	shell(f"esptool -p /dev/ttyUSB0 --chip esp32-s3 write-flash 0x10000 out/{target}-{fwver}-{commit}.bin")

@anno_command
def bf(project_name):
	"Build and flash specified project."
	build(project_name)
	flash(project_name)

main()
