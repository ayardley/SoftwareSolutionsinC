: 'run.sh -- Bourne shell script to test str algorithms on UNIX systems'

: 'Set the following to "sc=" to exclude the system C library strstr()'
sc=sc

make -f make.gen

./tststr || exit 1

for i in $sc ni bf kmp bm rk qs nm
do	echo ===== algorithm $i =====
	: date
	time ./repeat $i 20 < in.txt > out.$i
 	: date
done

rm out.nm

for i in $sc ni bf kmp bm rk qs
do	if cmp out.$i out.exp
	then	rm out.$i
	else	echo out.$i differs
	fi
done

make -f make.gen clobber
