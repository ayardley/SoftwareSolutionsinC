#!/usr/local/bin/perl

$NonWord= "\t ,;(\").\r\n";

while( <> ) {
    for $word (  split(/[$NonWord]/)  ) {
	$table{$word} ? delete $table{$word} : ++$table{$word};
    }
}

print "Words appearing an odd number of times:\n\t";
print join("\n\t",keys(%table)), "\n";
