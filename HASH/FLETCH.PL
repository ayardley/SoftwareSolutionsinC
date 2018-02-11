#!/usr/local/bin/perl
# fletch.pl -- Use Fletcher's checksum to compute 2-byte hash of string

sub fletch {
  local( $str )= @_;
  local( $sum1, $sum2, $check1 );
    for $ch (  split(//,$str)  ) {
	$sum1 += ord($ch);
	$sum1 -= 255   if  255 <= $sum1;
	$sum2 += $sum1;
    }
    $check1= $sum2;   $check1 %= 255;
    $check1= 255 - ($sum1+$check1) % 255;
    $sum1= 255 - ($sum1+$check1) % 255;
    ( $check1 << 8 )  |  $sum1;
}

$size= shift(@ARGV);
@ARGV  ||  die "Usage: $0 size str [str [...]]
	Size is hash table size.  Str's are values to be hashed.\n";
print 0+$size, " slots:";
for(  @ARGV  ) {
    print qq< "$_"=>, &fletch($_) % $size;
}
print "\n";
