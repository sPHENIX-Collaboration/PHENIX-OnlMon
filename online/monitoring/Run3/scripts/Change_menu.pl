#! /usr/bin/perl
# change the menu and menu.html files,
# replace the .txt with .txt.gz after all 
# txt files are gzipped

unless (open(P , "$ARGV[0]"))
{
  die "Couldn\'t open input file menu for reading: $!";
}

unless (open(Pout , " > menu.new"))
{
  die "Couldn\'t open output file menu for writing: $!";
}

while(<P>)
{
    $line = $_;
#    print "$line";
    $line =~ s/.txt/.txt.gz/g;
    print Pout "$line";

}

close (P);
close (Pout);
system("mv menu.new $ARGV[0]");
exit
