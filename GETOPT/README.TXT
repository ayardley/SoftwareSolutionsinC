	readme.txt -- Testing command-line option parsing functions

For systems resembling UNIX, edit "make.gen" as required, then invoke
"run.sh" to make "tstopt" and run the test, which should print on the
standard error output any problems detected.  You should also run
"tstopt" with a variety of bogus arguments to check error reporting.
(The run.* scripts don't do this, because that could obscure actual
implementation errors.)

If you don't have a UNIX-like shell and a suitable version of "make",
you must adapt the scripts appropriately.  (Peruse the Apple IIGS
scripts *.gs for ideas.)

The source code should work without change using any conforming
hosted implementation of Standard C.  It is assumed that lower-case
command-line arguments are supported (either in addition to upper-
case, or by mapping upper case to lower case).  If this assumption is
not true, you will have problems porting applications to your system!
