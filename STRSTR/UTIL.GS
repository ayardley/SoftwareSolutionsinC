*	util.gs -- ORCA shell script to build utility program on Apple IIGS
*		(assumes "import.gs" has already been executed)

*	last edit:	07-Jun-1993	Gwyn@ARL.Army.Mil

set	CFLAGS	-E -I +O -P +T +X

* Compile and link the utility:

unset	Exit
newer {1} {1}.cc
if {Status} != 0
	set	Exit	true
	cmpl {CFLAGS} {1}.cc keep={1}
end

* Clean up afterward:

delete -w {1}.a {1}.root
