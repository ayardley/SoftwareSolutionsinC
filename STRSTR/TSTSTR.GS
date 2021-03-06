*	tststr.gs -- ORCA shell script to build "tststr" on Apple IIGS
*		(assumes "import.gs" has already been executed)

*	last edit:	07-Jun-1993	Gwyn@ARL.Army.Mil

set	CFLAGS	-E -I +O -P +T +X
set	DEFINES	cc=(-dLOWAL=0)
* set	DEFINES	cc=(-dLOWAL=0 -dDEBUG=1)

set	MODULES	tststr bfstr bmstr kmpstr nistr qsstr rkstr

* Compile the sources. if necessary:

for i in {MODULES}
	unset	Exit
 	newer {i}.a {i}.cc
	if {Status} != 0
		set	Exit	true
		compile {CFLAGS} {i}.cc keep={i} {DEFINES}
	end
end

* Link objects into executable binary:

set	Exit	true

link {MODULES} keep=tststr

* Clean up afterward:

delete tststr.root tststr.a
