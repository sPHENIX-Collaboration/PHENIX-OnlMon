#!/usr/local/bin/perl

use strict;
use warnings;
use Env;
use File::Basename;
use File::Copy;
use File::Path;
use DBI;
use Getopt::Long;

my $update;
GetOptions("update"=>\$update);

if (defined $update)
{
    print "running in update mode\n";
}
else
{
    print "running in test mode, for real do check_html_runtype.pl -update\n";
}

if (!$ONLMON_HTMLDIR)
{
    die "No ONLMON_HTMLDIR env var, source setup_onlmon.csh";
}

my %htmloutdir = ();
$htmloutdir{"CALIBRATION"} = "calibdata";
$htmloutdir{"CREJECTED"} = "crejecteddata";
$htmloutdir{"PHYSICS"} = "eventdata";
$htmloutdir{"JUNK"} = "junkdata";
$htmloutdir{"PREJECTED"} = "prejecteddata";
$htmloutdir{"ZEROFIELD"} = "zerofielddata";
$htmloutdir{"VERNIERSCAN"} = "vernierscandata";
$htmloutdir{"LOCALPOLARIMETER"} = "localpoldata";
$htmloutdir{"PEDESTAL"} = "pedestaldata";
$htmloutdir{"PEDREJECTED"} = "pedrejecteddata";
$htmloutdir{"UNKNOWN"} = "unknowndata";

my $dbh = DBI->connect("dbi:ODBC:daq") || die $DBI::error;

my $getruntype = $dbh->prepare("select runtype from run where runnumber = ?");

open(F,"find $ONLMON_HTMLDIR/ -type d | ");

my $datadir = "unknowndata";
while(my $dir = <F>)
{
    chomp $dir;
    my $base = basename($dir);
    if ($base =~ /data/)
    {
	$datadir = $base;
    }
# okay stupid test to find out if the dir is the runnumber, not
# some run_XXX_XXX
    if ($base =~ /(\d+)/)
    {
	my $runnumber = $1;
	if ($base eq $runnumber)
	{
	    $getruntype->execute($runnumber) || die $DBI::error;
	    my $runtype = "UNKNOWN";
	    if ($getruntype->rows > 0)
	    {
		my @rtype = $getruntype->fetchrow_array();
		$runtype = $rtype[0];
	    }
#	    print "run $runnumber, runtype $runtype, current base: $datadir\n";
	    if (! exists $htmloutdir{$runtype})
	    {
		die "no html out directory defined for $runtype\n";
	    }
	    if ($htmloutdir{$runtype} ne $datadir)
	    {
		print "need to move run $runnumber from $datadir to $htmloutdir{$runtype}\n";
		my $lower = $runnumber - $runnumber%1000;
		my $upper = $lower + 1000;
		my $rundir = sprintf("%s/%s/run_%010d_%010d",$ONLMON_HTMLDIR,$htmloutdir{$runtype},$lower,$upper);
#		print "my dir: $dir\n";
#		print "new rundir: $rundir\n";
		my $newdir = $rundir . "/" . $runnumber;
		if ( -d $newdir)
		{
		    print "rundir $newdir already exists\n";
		}

		if (defined $update)
		{
		    if (! -d $rundir)
		    {
			print "creating $rundir\n";
			mkpath($rundir);
		    }
		    my $newdir = $rundir . "/" . $runnumber;
		    if ( -d $newdir)
		    {
			print "rundir $newdir already exists, that run needs special treatment\n";
		    }
		    else
		    {
			print "moving $dir to $newdir\n";
		        move($dir,$newdir);
		    }
		}

	    }
	}
	else
	{
#	    print "no good $dir\n";
	}
    }
}
close(F);

$getruntype->finish();
$dbh->disconnect;


