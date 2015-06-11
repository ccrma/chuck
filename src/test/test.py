#!/usr/bin/python

import sys
import os
import subprocess

failures = 0
successes = 0;


def handle_directory(dir, exe):
    print ""
    print ">>> Performing tests in %s <<<" % dir

    global successes

    for filename in os.listdir(dir):
        path = os.path.join(dir, filename)
        if os.path.isfile(path):
            if os.path.splitext(filename)[1] == ".ck":
                print "> %s %s" % (exe, path)
                try:
                    result = subprocess.check_output([exe, "--silent", "%s" % path], stderr=subprocess.STDOUT)
                    
                    if not result.strip().endswith(("\"success\" : (string)",)):
                        if os.path.isfile(path.replace(".ck", ".txt")):
                            # print "\tChecking result with answer file: " + path.replace(".ck", ".txt")

                            with open(path.replace(".ck", ".txt")) as answerFile:
                                answer = answerFile.read()

                            if answer != result:
                                fail(filename, result)
                            else:
                                successes += 1
                        else:
                            fail(filename, result)
                    else:
                        successes += 1;
                except subprocess.CalledProcessError as e:
                    fail(filename, e.output)
        elif os.path.isdir(path) and filename[0] != '.':
            handle_directory(path, exe)


def fail(testName, output):
    global failures
    print "*** Test '%s' failed: ***" % testName
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

print ""

if failures == 0:
    print "Success - all " + str(successes) + " tests passed!"
    sys.exit(0)
else:
    print "Failure - " + str(failures) + " test(s) failed"
    sys.exit(-1)
