#!/usr/bin/python

import sys
import os
import subprocess
import time


failures = 0
successes = 0


def handle_directory(dir, exe):
    print ""
    print ">>> Performing tests in %s <<<" % dir

    global successes

    for filename in os.listdir(dir):
        path = os.path.join(dir, filename)
        if os.path.isfile(path):
            if os.path.splitext(filename)[1] == ".ck":
                run_test(exe, path, filename, 0)
        elif os.path.isdir(path) and filename[0] != '.':
            handle_directory(path, exe)


def run_test(exe, path, filename, attempt):
    global successes
    print "> %s %s" % (exe, path)

    try:
        result = subprocess.check_output([exe, "--silent", "%s" % path], stderr=subprocess.STDOUT)

        if not result.strip().endswith(("\"success\" : (string)",)):
            if os.path.isfile(path.replace(".ck", ".txt")):
                # print "\tChecking result with answer file: " + path.replace(".ck", ".txt")

                with open(path.replace(".ck", ".txt")) as answerFile:
                    answer = answerFile.read()

                if answer != result:
                    handle_failure(exe, path, filename, attempt, result)
                else:
                    successes += 1
            else:
                handle_failure(exe, path, filename, attempt, result)

        else:
            successes += 1
    except subprocess.CalledProcessError as e:
        handle_failure(exe, path, filename, attempt, e.output)


def handle_failure(exe, path, filename, attempt, error_string):
    # We'll attempt up to run the test at most this more times
    retry_attempts_for_failed_tests = 2

    # Between failed runs, let things sleep a little to cool off
    sleep_interval_between_runs_seconds = 2

    if attempt < retry_attempts_for_failed_tests:
        time.sleep(sleep_interval_between_runs_seconds)
        print "Retrying test: %s %s" % (exe, path)
        run_test(exe, path, filename, attempt + 1)
    else:
        fail(filename, error_string)


def fail(test_name, output):
    global failures
    print "*** Test '%s' failed: ***" % test_name
    print output
    failures += 1


def main():
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


if __name__ == "__main__":
    main()
