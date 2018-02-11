*	make.gs -- ORCA shell script to build "tstopt" for Apple IIGS

*	last edit:	30-May-1993	Gwyn@ARL.Army.Mil

set	CFLAGS	-E -I +O -P +T +X
unset	DEFINES
* set	DEFINES	cc=(-dDEBUG=1 -dRIGID_SPEC=1 -dROBUST=1 -dSET_OPTOPT=1)
set	MODULES	tstopt getopt getsub

* Rename the source files, give them CC filetype, then compile them:

for i in {MODULES}
	unset	Exit
 	exists {i}.c
	if {Status} != 0
		set	Exit	true
		delete -w {i}.cc
		rename {i}.c {i}.cc
		change {i}.cc cc
	end

	unset	Exit
 	newer {i}.a {i}.cc
	if {Status} != 0
		set	Exit	true
		compile {CFLAGS} {i}.cc keep={i} {DEFINES}
	end
end

set	Exit	true

link {MODULES} keep=tstopt
