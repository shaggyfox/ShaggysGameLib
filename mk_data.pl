#!/usr/local/bin/perl

open($in, "<", $ARGV[0]);
open($out, ">", $ARGV[1]);

$c = 0;
$i = 0;

printf $out "static char %s[] = {\n", $ARGV[2] ? $ARGV[2] : "data";

while (read $in, $c, 1){
  printf $out "0x%02X,", ord($c);
  $i = $i + 1;
  if ($i >= 20) {
    printf $out "\n";
    $i = 0;
  }
}

printf $out "};\n";

close $in;
close $out;

