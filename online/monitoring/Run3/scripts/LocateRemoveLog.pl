#!/usr/local/bin/perl
# this script goes over the logfiles which are saved in
# $ONLMON_LOGDIR and locates and compares them to the
# files found in $ONLMON_HTMLDIR which should contain
# an identical copies of them if the html generation went fine.
# It removes the files which are found to be okay

use strict;
use Env;
use Getopt::Long;
use File::stat;

if (!$ONLMON_LOGDIR)
{
    die "No ONLMON_LOGDIR env var, source setup_onlmon.csh";
}
if (!$ONLMON_HTMLDIR)
{
    die "No ONLMON_HTMLDIR env var, source setup_onlmon.csh";
}

my $dounlink;

GetOptions("kill"=>\$dounlink);

#find data dirs
my @datadirs = ();
open(F,"find $ONLMON_HTMLDIR -maxdepth 1 -type d -name '*data' | ");
while(my $datdir=<F>)
{
    chomp $datdir;
    push(@datadirs,$datdir);
}

close(F);

open(F,"find $ONLMON_LOGDIR -name '*.log.gz' |");

while(my $file = <F>)
{
    chomp $file;
    my @sp1 = split(/\//,$file);
    my $logfile=$sp1[$#sp1];
    my $newname=$logfile;
    $newname =~ s/\.log\./\.txt\./;
    $newname =~ s/\_/\_log\_/;
    my $runnumber;
    if ($logfile =~ /.*_(\d+)\..*/)
    {
	$runnumber = $1;
    }
    else
    {
	die "could not extract runnumber from $logfile\n";
    }
    my $lower = $runnumber - $runnumber%1000;
    my $upper = $lower + 1000;
    my $rundir = sprintf("run_%010d_%010d", $lower,  $upper);
    my $foundit=0;
    my $fullname;
    for (my $n=0; $n<=$#datadirs;$n++)
    {
	$fullname=$datadirs[$n] . "/" . $rundir . "/" . $runnumber . "/" . $newname;
	if (-e $fullname)
	{
	    $foundit = 1;
	    last;
	}
    }

    if ($foundit == 1)
    {
	system("diff $file $fullname");
	my $exit_value  = $? >> 8;
	if ($exit_value != 0)
	{
	    my $f1size = stat($file)->size;
	    my $f2size = stat($fullname)->size;
	    print "diff failed for diff $file $fullname\n";
	    print "size $file: $f1size\n";
	    print "size $fullname: $f2size\n";
	    die;
	}
	else
	{
	    if (defined $dounlink)
	    {
		print "$file and $fullname are identical, removing $file\n"; 
		unlink $file;
	    }
	    else
	    {
		print "$file and $fullname are identical, would remove $file\n"; 
	    }
	}
    }
    else
    {
	print "logfile $logfile found only in ONLMON_LOGDIR, not in html dirs\n";
    }
}

close(F);

