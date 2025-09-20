"""
Helper script for remembering common build commands in my personal development on this repo.
"""

import os
import sys
import subprocess
import traceback
import datetime

cmds = dict()
projaliases = {
	'fox': 'Heltec_v3_user_cparen_foxradio',
	'comp': 'Heltec_v3_companion_radio_ble'
}

def main():
	setup_vars()
	try:
		if len(sys.argv) == 1 or sys.argv[1] == '-h':
			show_usage();
		else:
			do_command(sys.argv[1], *sys.argv[2:])
	except Exception:
		traceback.print_exc()
		sys.exit(1)

def setup_vars():
	global COMMIT_HASH, FIRMWARE_BUILD_DATE, FIRMWARE_VERSION, FIRMWARE_VERSION_STRING
	COMMIT_HASH=shellcapture("git rev-parse --short HEAD")
	FIRMWARE_BUILD_DATE=datetime.date.today().strftime('+%d-%b-%Y-t%H%M')
	FIRMWARE_VERSION = os.environ["FIRMWARE_VERSION"]
	FIRMWARE_VERSION_STRING=f"{FIRMWARE_VERSION}-{COMMIT_HASH}"

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
	if ec: raise Exception(f"failed with exit code {ec}")

def shellcapture(command_string):
	return subprocess.check_output(command_string, shell=True, text=True).strip()

def do_command(command, *argv):
	if command not in cmds:
		raise Exception("command not found")
	cmds[command](*argv)

@anno_command
def wh(command):
	"(wh)at: Show docstring for the command"
	if command not in cmds:
		raise Exception("command not found")
	print(cmds[command].__doc__)

@anno_command
def pa():
	"Print available project aliases"
	print("available projects to build or flash")
	for k,v in projaliases:
		print(f"{k}: {v}")

@anno_command
def build(project_name):
	"""
	Build a project firmware. 

	Use 'pa' to see list of project aliases.
	Based on build.sh in root of repo.
	"""
	target = projaliases[project_name]
	FIRMWARE_FILENAME=f"{target}-{FIRMWARE_VERSION_STRING}"
	PLATFORMIO_BUILD_FLAGS=f"-DFIRMWARE_BUILD_DATE='\"{FIRMWARE_BUILD_DATE}\"' -DFIRMWARE_VERSION='\"{FIRMWARE_VERSION_STRING}\"'"

	shell(f"pio run -e {target}")
	shell(f"pio run -t mergebin -e {target}")
	 

@anno_command
def flash(project_name):
	"""
	Flash a resulting firmware.

	Use 'pa' to see list of project aliases.
	Based on build.sh in root of repo.
	"""
	target = projaliases[project_name]
	FIRMWARE_FILENAME=f"{target}-{FIRMWARE_VERSION_STRING}"

	shell(f"esptool -p /dev/ttyUSB0 --chip esp32-s3 write-flash 0x10000 out/{FIRMWARE_FILENAME}.bin")

@anno_command
def bf(project_name):
	"Build and flash specified project."
	build(project_name)
	flash(project_name)

@anno_command
def comtty():
	"Connect to serial port with screen. Use ctrl-a ctrl-k to close."
	shell("screen /dev/ttyUSB0 115200")

@anno_command
def restore():
	"Restore downloaded companion bluetooth firmware"
	shell("esptool -p /dev/ttyUSB0 --chip esp32-s3 write-flash 0x10000 ~/_dl/Heltec_v3_companion_radio_ble-v1.8.1-1130cf1.bin")

main()
