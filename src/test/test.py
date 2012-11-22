#!/usr/bin/python

import sys
import os
import subprocess

failures = 0

def handle_directory(dir, exe):
    print ">>> Performing tests in %s <<<" % dir
    for filename in os.listdir(dir):
        path = os.path.join(dir, filename)
        if os.path.isfile(path):
            if os.path.splitext(filename)[1] == ".ck":
                print "> %s %s" % (exe, path)
                try:
                    result = subprocess.check_output([exe, "%s" % path], stderr=subprocess.STDOUT)
                    if result != "\"success\" : (string)\n":
                        fail(filename, result)
                except subprocess.CalledProcessError as e:
                    fail(filename, e.output)
        elif os.path.isdir(path) and filename[0] != '.':
            handle_directory(path, exe)

def fail(testname, output):
    global failures
    print "*** test '%s' failed: ***" % testname
    print output
    #sys.exit(-1)
    failures += 1

###

exe = 'chuck'
test_dir = '.'

if len(sys.argv) >= 2:
    exe = sys.argv[1]
if len(sys.argv) >= 3:
    test_dir = sys.argv[2]

handle_directory(test_dir, exe)

if failures == 0:
    print "all success!"
sys.exit(failures)
