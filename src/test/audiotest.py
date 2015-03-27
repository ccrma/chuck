#!/usr/bin/python

import sys, os, subprocess, ConfigParser

failures = 0
successes = 0;

def handle_directory(dir, exe, recck, audiotest):
    print ""
    print ">>> Performing tests in %s <<<" % dir

    global successes

    for filename in os.listdir(dir):
        path = os.path.join(dir, filename)
        path = os.path.abspath(path)
        if os.path.isfile(path):
            if os.path.splitext(filename)[1] == ".cfg":
                try:
                    cfg = ConfigParser.RawConfigParser()
                    cfg.read(path)
                    
                    ckpath = os.path.join(dir, cfg.get('audiotest', 'chuck_file'))
                    comparepath = os.path.join(dir, cfg.get('audiotest', 'compare_file'))
                    duration = cfg.getfloat('audiotest', 'duration')
                    # options = cfg.get('audiotest', 'options').split(' ')
                    mode = cfg.get('audiotest', 'mode')
                    recpath = os.path.splitext(comparepath)[0] + '-tmp.wav'
                    
                    invocation = [exe, "--silent", "%s" % ckpath, recck % (recpath, duration)]
                    print "> %s" % ' '.join(invocation)
                    result = subprocess.call(invocation, stderr=subprocess.STDOUT)
                    
                    if result == 0:
                        invocation = [audiotest, '-%s'%mode, comparepath, recpath]
                        print "> %s" % ' '.join(invocation)
                        result = subprocess.call(invocation)
                        if result == 0:
                            success += 1
                        else:
                            fail(filename, 'audiotest returned error code %i' % result)
                        invocation = ['rm', recpath]
                        print "> %s" % ' '.join(invocation)
                        # subprocess.call(invocation)
                    else:
                        fail(filename, 'chuck returned error code %i' % result)
                except subprocess.CalledProcessError as e:
                    fail(filename, e.output)
        elif os.path.isdir(path) and filename[0] != '.':
            handle_directory(path, exe, recck, audiotest)


def fail(testName, output):
    global failures
    print "*** Test '%s' failed: ***" % testName
    print output
    #sys.exit(-1)
    failures += 1

###

exe = 'chuck'
audiotest = 'audiotest/audiotest'
recck = 'rec.ck:%s:%f'
test_dir = '.'

if len(sys.argv) >= 2:
    exe = sys.argv[1]
if len(sys.argv) >= 3:
    test_dir = sys.argv[2]

handle_directory(test_dir, exe, recck, audiotest)

print ""

if failures == 0:
    print "Success - all " + str(successes) + " tests passed!"
    sys.exit(0)
else:
    print "Failure - " + str(failures) + " test(s) failed"
    sys.exit(-1)
