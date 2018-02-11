These ORCA/C sources for use with Apple's GS/OS are probably incomplete; they
were extracted from a very large POSIX-compatible C library implementation.
They are included because they illustrate a certain common kind of operating
system interface to directories.  The pathname canonicalization code in
q8cano.cc might be useful for other implementations as well.

The process of changing filenames to fit MS-DOS disk requirements interferes
with the intended installation procedure anyway.  If you're interested in the
complete original GS/OS library from which this code was extracted (I call it
"GPLib"), feel free to drop me a note:

	Douglas A. Gwyn
	U.S. Army Research Laboratory
	AMSRL-CI-CC
	Aberdeen Proving Ground, MD 21005-5067

	(410)278-8945

	Gwyn@ARL.Army.Mil
