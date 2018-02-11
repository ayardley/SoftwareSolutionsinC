*	run.gs (ORCA shell script) - verify operation
*	First type "change run.gs exec", then "run.gs".

*	last edit:	19-Jan-1994	Gwyn@ARL.Army.Mil

set	Exit	true

change make.gs exec
change pack.gs exec

make.gs

* Test correctness:

tstopt
tstopt --
tstopt -
tstopt getopt.cc
tstopt -- getopt.cc
tstopt - getopt.cc
tstopt -- - getopt.cc
tstopt getopt.cc -
tstopt -- getopt.cc -
tstopt getopt.cc readme.txt
tstopt -- getopt.cc readme.txt
tstopt -a
tstopt -a --
tstopt -a -
tstopt -a getopt.cc
tstopt -a -- getopt.cc
tstopt -a - getopt.cc
tstopt -a -- - getopt.cc
tstopt -a getopt.cc -
tstopt -a -- getopt.cc -
tstopt -a getopt.cc readme.txt
tstopt -- - -
tstopt -- - - getopt.cc
tstopt -- getopt.cc - readme.txt
tstopt -- getopt.cc - readme.txt -
tstopt -b
tstopt -b --
tstopt -b -
tstopt -b getopt.cc
tstopt -b -- getopt.cc
tstopt -b - getopt.cc
tstopt -b -- - getopt.cc
tstopt -b getopt.cc -
tstopt -b -- getopt.cc -
tstopt -b getopt.cc readme.txt
tstopt -ab
tstopt -ab --
tstopt -ab -
tstopt -ab getopt.cc
tstopt -ab - getopt.cc
tstopt -ab -- getopt.cc
tstopt -a -b
tstopt -a -b --
tstopt -a -b -
tstopt -a -b getopt.cc
tstopt -a -b - getopt.cc
tstopt -a -b -- getopt.cc
tstopt -i readme.txt
tstopt -i readme.txt --
tstopt -i readme.txt -
tstopt -i readme.txt getopt.cc
tstopt -i readme.txt - getopt.cc
tstopt -i readme.txt -- getopt.cc
tstopt -a -i readme.txt
tstopt -a -i readme.txt --
tstopt -a -i readme.txt -
tstopt -a -i readme.txt getopt.cc
tstopt -a -i readme.txt - getopt.cc
tstopt -a -i readme.txt -- getopt.cc
tstopt -i readme.txt -a
tstopt -i readme.txt -a --
tstopt -i readme.txt -a -
tstopt -i readme.txt -a getopt.cc
tstopt -i readme.txt -a - getopt.cc
tstopt -i readme.txt -a -- getopt.cc
tstopt -o c
tstopt -o d --
tstopt -o name=junk -
tstopt -o c,name=junk getopt.cc
tstopt -o name=junk,d - getopt.cc
tstopt -o d,name=junk,c -- getopt.cc
tstopt -i readme.txt -o c
tstopt -i readme.txt -o d --
tstopt -i readme.txt -o name=junk -
tstopt -i readme.txt -o c,name=junk getopt.cc
tstopt -i readme.txt -o name=junk,d - getopt.cc
tstopt -i readme.txt -o d,name=junk,c -- getopt.cc
tstopt -a -o c
tstopt -a -o d --
tstopt -a -o name=junk -
tstopt -a -o c,name=junk getopt.cc
tstopt -a -o name=junk,d - getopt.cc
tstopt -a -o d,name=junk,c -- getopt.cc
tstopt -o c -i readme.txt -a
tstopt -o d -i readme.txt -a --
tstopt -o name=junk -i readme.txt -a -
tstopt -o c,name=junk -i readme.txt -a getopt.cc
tstopt -o name=junk,d -i readme.txt -a - getopt.cc
tstopt -o d,name=junk,c -i readme.txt -a -- getopt.cc
tstopt -o c -b -i readme.txt
tstopt -o d -b -i readme.txt --
tstopt -o name=junk -b -i readme.txt -
tstopt -o c,name=junk -b -i readme.txt getopt.cc
tstopt -o name=junk,d -b -i readme.txt - getopt.cc
tstopt -o d,name=junk,c -b -i readme.txt -- getopt.cc

* Clean up afterward

delete junk
delete tstopt =.root =.a
