#!/usr/local/bin/perl

use File::Basename;
use File::stat;
use strict;
use warnings;
use Getopt::Long;

if ( $#ARGV < 0 )
{

    print "usage: safe_delete.pl top_dir_to_delete top_dir_in_hpss\n";
    print "flags:\n";
    print "-kill   remove file for real\n";
    exit(-1);
}
my $dir=$ARGV[0];
my $hpsstopdir=$ARGV[1];

my $dokill;

GetOptions('kill'=>\$dokill);
if (! defined $dokill)
{
    print "TestMode, use -kill to delete files for real\n";
}

open(F,"find $dir/ -type f -print |");

my $nr_files=0;

while ( my $file = <F> )
{
    chomp $file;
    my $fn=basename($file);
    my $filedir = dirname($file);
    my $hpssdir = $filedir;
    $hpssdir =~ s/$dir/$hpsstopdir/;
    my $hpssfile = sprintf("%s/%s",$hpssdir,$fn);
    my $size = &get_size($file);
    my $hsize = &get_hpss_size($hpssfile);
    if ($hsize < 0) # error from subroutine - go to next file
    {
	next;
    }
    if($size==$hsize)
    {
	$nr_files++;
	if (defined $dokill)
	{
	    print "HPSS size:$hsize, disk size:$size, Deleting $file\n";
	    unlink $file;
	}
	else
	{
	    print "HPSS size:$hsize, disk size:$size, would delete $file\n";
	}
    } 
    else
    {
	print "HPSS size does not match: $file\n";
    }
}

close(F);
if (defined $dokill)
{
    print "Number of files deleted: $nr_files\n";
}
else
{
    print "Test Mode, would delete $nr_files files\n";
}

sub get_size
{
    my $f = shift;
    my $size = stat($f)->size;
    return $size;
}

sub get_hpss_size
{
    my $hpssfile=shift;
    open(F1,"hpssget hpssbat.rcf.bnl.gov 10242 statx $hpssfile 2>&1 |");
    my $hsize = -1;
    while(my $line = <F1>)
    {
        chomp $line;
        if ($line =~ /ERROR/)
        {
            print "hpssget error for $hpssfile\n";
            $hsize = -2;
            last;
        }
        my @sp1 = split(/ /,$line);
        $hsize = $sp1[$#sp1];
    }
    close(F1);
    return $hsize;
}
