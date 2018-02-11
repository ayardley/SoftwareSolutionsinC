: 'run.sh -- Bourne shell script to verify operation'

make -f make.gen

./tstopt
./tstopt --
./tstopt -
./tstopt getopt.c
./tstopt -- getopt.c
./tstopt - getopt.c
./tstopt -- - getopt.c
./tstopt getopt.c -
./tstopt -- getopt.c -
./tstopt getopt.c readme.txt
./tstopt -- getopt.c readme.txt
./tstopt -a
./tstopt -a --
./tstopt -a -
./tstopt -a getopt.c
./tstopt -a -- getopt.c
./tstopt -a - getopt.c
./tstopt -a -- - getopt.c
./tstopt -a getopt.c -
./tstopt -a -- getopt.c -
./tstopt -a getopt.c readme.txt
./tstopt -- - -
./tstopt -- - - getopt.c
./tstopt -- getopt.c - readme.txt
./tstopt -- getopt.c - readme.txt -
./tstopt -b
./tstopt -b --
./tstopt -b -
./tstopt -b getopt.c
./tstopt -b -- getopt.c
./tstopt -b - getopt.c
./tstopt -b -- - getopt.c
./tstopt -b getopt.c -
./tstopt -b -- getopt.c -
./tstopt -b getopt.c readme.txt
./tstopt -ab
./tstopt -ab --
./tstopt -ab -
./tstopt -ab getopt.c
./tstopt -ab - getopt.c
./tstopt -ab -- getopt.c
./tstopt -a -b
./tstopt -a -b --
./tstopt -a -b -
./tstopt -a -b getopt.c
./tstopt -a -b - getopt.c
./tstopt -a -b -- getopt.c
./tstopt -i readme.txt
./tstopt -i readme.txt --
./tstopt -i readme.txt -
./tstopt -i readme.txt getopt.c
./tstopt -i readme.txt - getopt.c
./tstopt -i readme.txt -- getopt.c
./tstopt -a -i readme.txt
./tstopt -a -i readme.txt --
./tstopt -a -i readme.txt -
./tstopt -a -i readme.txt getopt.c
./tstopt -a -i readme.txt - getopt.c
./tstopt -a -i readme.txt -- getopt.c
./tstopt -i readme.txt -a
./tstopt -i readme.txt -a --
./tstopt -i readme.txt -a -
./tstopt -i readme.txt -a getopt.c
./tstopt -i readme.txt -a - getopt.c
./tstopt -i readme.txt -a -- getopt.c
./tstopt -o c
./tstopt -o d --
./tstopt -o name=junk -
./tstopt -o c,name=junk getopt.c
./tstopt -o name=junk,d - getopt.c
./tstopt -o d,name=junk,c -- getopt.c
./tstopt -i readme.txt -o c
./tstopt -i readme.txt -o d --
./tstopt -i readme.txt -o name=junk -
./tstopt -i readme.txt -o c,name=junk getopt.c
./tstopt -i readme.txt -o name=junk,d - getopt.c
./tstopt -i readme.txt -o d,name=junk,c -- getopt.c
./tstopt -a -o c
./tstopt -a -o d --
./tstopt -a -o name=junk -
./tstopt -a -o c,name=junk getopt.c
./tstopt -a -o name=junk,d - getopt.c
./tstopt -a -o d,name=junk,c -- getopt.c
./tstopt -o c -i readme.txt -a
./tstopt -o d -i readme.txt -a --
./tstopt -o name=junk -i readme.txt -a -
./tstopt -o c,name=junk -i readme.txt -a getopt.c
./tstopt -o name=junk,d -i readme.txt -a - getopt.c
./tstopt -o d,name=junk,c -i readme.txt -a -- getopt.c
./tstopt -o c -b -i readme.txt
./tstopt -o d -b -i readme.txt --
./tstopt -o name=junk -b -i readme.txt -
./tstopt -o c,name=junk -b -i readme.txt getopt.c
./tstopt -o name=junk,d -b -i readme.txt - getopt.c
./tstopt -o d,name=junk,c -b -i readme.txt -- getopt.c

rm junk
make -f make.gen clobber
