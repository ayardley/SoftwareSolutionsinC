:
# warn.sh -- Generate compiler warnings for final code

input=sample
if  test -f $input  -a  ! -f perlsort;  then
    ./hashtest.pl <$input | sort >perlsort
fi

rm -f gcc.log
cnt=0
for d in "" -DHASH_PASS_LENGTH -DSIZE_TWO_POWER \
	"-DHASH_PASS_LENGTH -DSIZE_TWO_POWER"; do
    cnt=`expr 1 + $cnt`
    gcc -I. -Wall -c -DDEBUG $d *.c 2>> gcc.log
    gcc -I. -Wall -o hashtest.$cnt $d *.c 2>> gcc.log
    if  test -f $input  -a  $? -eq 0;  then
	./hashtest.$cnt <$input | sort | diff - perlsort | head -500 >>gcc.log
    fi
done
