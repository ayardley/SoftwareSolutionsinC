*	run.gs -- ORCA shell script to test str algorithms on Apple IIGS
*	First type "change run.gs exec", then "run.gs".

*	last edit:	07-Jun-1993	Gwyn@ARL.Army.Mil

set	Exit	true

* change export.gs exec
change import.gs exec
change pack.gs exec
change repeat.gs exec
change tststr.gs exec
change util.gs exec

import.gs
util.gs cmp
util.gs date
repeat.gs
tststr.gs

* set sc to "sc" to include a test of the Standard=C strstr()
set	sc	sc
* unset	sc

set	Exit	true

* Test correctness:

tststr

delete tststr

* Time each algorithm and verify correctness of results:

for i in {sc} ni bf kmp bm rk qs
	set	Exit	true
	echo ::::: algorithm {i} :::::
	date
	repeat {i} 20 < in.txt > out.{i}
	date
	unset	Exit
	cmp out.{i} out.exp
	if {Status} = 0
		delete out.{i}
	else
		echo out.{i} differs
	end
end

set	Exit	true
echo ::::: algorithm nm :::::
date
repeat nm 20 < in.txt > out.nm
date

delete out.nm

* Clean up afterward

delete =.a =.root
delete cmp date repeat
