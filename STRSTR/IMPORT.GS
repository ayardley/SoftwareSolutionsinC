*	import.gs -- ORCA shell script to rename files on Apple IIGS

*	last edit:	07-Jun-1993	Gwyn@ARL.Army.Mil

set	MODULES	bfstr bmstr cmp date kmpstr nistr nmstr qsstr repeat rkstr
set	MODULES	{MODULES} snstr tststr

* Rename the source files and give them CC filetype, if necessary:

for i in {MODULES}
	unset	Exit
 	exists {i}.c
	if {Status} != 0
		set	Exit	true
		rename {i}.c {i}.cc
		change {i}.cc cc
	end
end
