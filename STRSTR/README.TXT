	readme.txt -- Timing the various string-search algorithms

For systems resembling UNIX, edit "make.gen" as required, then invoke
"run.sh" to make "repeat" and "tststr" and run the test, which should
print on the standard output time taken by each algorithm.  The test
runs take several seconds even on the fastest machines, so on slow
machines be prepared to wait for minutes or perhaps even hours.  The
file "times.txt" shows timing results for each algorithm on several
different types of computer system.

If you don't have a UNIX-like shell and a suitable version of "make",
you must adapt the scripts appropriately.  (Peruse the Apple IIGS
scripts *.gs for ideas.)

The source code should work without change using any conforming
hosted implementation of Standard C.  If you don't have a UNIX
system, you may need to adapt the run.* script, for example to invoke
a timestamp utility (like the "date" program, source code for which is
included in case your system doesn't have a suitable command) before
and after execution of each algorithm and taking the difference in
timestamps as the execution time.  Note: On multitasking systems, the
elapsed "real" time varies with system load; if your system does not
provide process-specific timing, then you'll need to run the test on a
quiescent system (i.e. one without any other competing activity -- no
desk accessories, printer spooler, network handling, etc.).  The "cmp"
utility is used to validate outputs; its source code is included in
case you don't have a comparable command on your system.

To include the system C library implementation of strstr() in the test,
LOWAL must be predefined as 0 when compiling, and the run.* script must
define the shell variable "sc" to have the value "sc".  (This is how
these files are configured as distributed.)
