#!/usr/bin/python

import sys

if len(sys.argv) != 2:
    print "usage: %s ugen_name" % sys.argv[0]
    sys.exit(-1)

ugen = sys.argv[1]

code = "%s u => blackhole;\n\
1::samp => now;\n\
u =< blackhole;\n\
null @=> u;\n\
\n\
<<< \"success\" >>>;\n\
";

f = open('%s.ck' % ugen, 'w');
f.write(code % ugen);

f.close();
