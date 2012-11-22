#!/usr/bin/python

import sys
import os
import subprocess

failures = 0

def handle_directory(dir, exe):
    for dirname, dirnames, filenames in os.walk(dir):
        for filename in filenames:
            if os.path.splitext(filename)[1] == ".ck":
                path = os.path.join(dir, filename)
                print "> %s %s" % (exe, path)
                try:
                    result = subprocess.check_output([exe, "%s" % path], stderr=subprocess.STDOUT)
                    if result != "\"success\" : (string)\n":
                        fail(filename, result)
                except subprocess.CalledProcessError as e:
                    fail(filename, e.output)
            
        for subdirname in dirnames:
            handle_directory(os.path.join(dirname, subdirname), exe)

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
