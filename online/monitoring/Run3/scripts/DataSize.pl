#!/usr/local/bin/perl -w

use DBI;
use strict;
use Env;

if ($#ARGV < 0)
{
    die "usage: DataSize.pl <runnumber>\n";
}

my $runnumber = $ARGV[0];

#print "ONLINE_CONFIGURATION: $ONLINE_CONFIGURATION\n";

my %pcffilelist = ();
my %dcmgroups = ();
my %fibergroups = ();
my %dcmgrouppacketmap = ();
my %fibergrouppacketmap = ();

sub findpcffiles
{
    my $granuledef = $ONLINE_CONFIGURATION . "/rc/hw/granuleDef.pcf";
    open(F,"$granuledef");
    while(my $line = <F>)
    {
	chomp $line;
	if ($line =~ /label/ && $line =~ /RC_HW_CONF/)
	{
	    my @sp1 = split(/RC_HW_CONF\//,$line);
	    my $pcffile = $ONLINE_CONFIGURATION . "/rc/hw/" . $sp1[$#sp1];
#	    print "$sp1[$#sp1]\n";
	    if (-f $pcffile && $pcffile !~ /emc.tb.pcf/)
	    {
		$pcffilelist{$pcffile} = 1;
	    }
	}
    }
    close(F);
}

findpcffiles();
foreach my $pcffile (keys %pcffilelist)
{
#    print "$pcffile\n";
    open(F,$pcffile);
    my $dcmgrp = "NONE";
    my $fibergrp = "NONE";
    while(my $line = <F>)
    {
	chomp $line;
	if ($line =~ /DCMGROUP/ && $line =~ /level1dd/)
	{
	    my @sp1 = split(/DCMGROUP/,$line);
	    my @sp2 = split(/,/,$sp1[$#sp1]);
	    $dcmgrp = "DCMGROUP" . $sp2[0];
#	    print "$dcmgrp\n";
	}
	if ($line =~ /unit/)
	{
	    my @sp1 = split(/unit/,$line);
	    my @sp2 = split(/,/,$sp1[$#sp1]);
	    $fibergrp = $dcmgrp . $sp2[0];
#	    print "$fibergrp\n";

	}
	if ($line =~ /packetid/)
	{
	    my @sp1 = split(/,/,$line);
	    for (my $i=0; $i<= $#sp1; $i++)
	    {
		if ($sp1[$i] =~ /packetid/)
		{
		    my @sp2 = split(/:/,$sp1[$i]);
		    my @sp3 = split(/,/,$sp2[$#sp2]);
		    if ($sp3[0] > 0)
		    {
			$dcmgrouppacketmap{$sp3[0]} = $dcmgrp;
			if (!exists $dcmgroups{$dcmgrp})
			{
			    $dcmgroups{$dcmgrp} = 0;
			}
			$fibergrouppacketmap{$sp3[0]} = $fibergrp;
			if (!exists $fibergroups{$fibergrp})
			{
			    $fibergroups{$fibergrp} = 0;
			}
		    }
		}
	    }
	}
    }
}

my $driver = "Pg";
my $database = "OnlMonDB";
my $hostname = "phnxdb0.phenix.bnl.gov";
my $user = "phnxrc";
my $password = "";

my $dsn = "dbi:$driver:dbname=$database;host=$hostname";

my $dbh = DBI->connect($dsn, $user, $password, {PrintError=>0}) || die $DBI::error;
my $gettables = $dbh->prepare("select tablename from pg_tables where tablename like 'pktsizemon%'");
$gettables->execute() || die $DBI::error;
while(my @tables = $gettables->fetchrow_array())
{
    my $getpackets = $dbh->prepare("select * from $tables[0] where runnumber = ?");
    $getpackets->execute($runnumber);
    if ($getpackets->rows > 0)
    {
	my @ptks = $getpackets->fetchrow_array();
	for (my $n = 0; $n < $getpackets->{NUM_OF_FIELDS}; $n++)
	{
	    my $column = $getpackets->{NAME}->[$n];
	    if ($column =~ /p_/)
	    {
		my @sp1 = split(/p_/,$column);
		my $pktid = $sp1[$#sp1];
		if (exists $dcmgrouppacketmap{$pktid})
		{
		    $dcmgroups{$dcmgrouppacketmap{$pktid}} += $ptks[$n];

#		    print "Adding $ptks[$n] to $dcmgrouppacketmap{$pktid}\n";
		}
		if (exists $fibergrouppacketmap{$pktid})
		{
		    $fibergroups{$fibergrouppacketmap{$pktid}} += $ptks[$n];
		}
	    }
	}
    }
    $getpackets->finish();
}
foreach my $entry (sort keys %dcmgroups)
{
    print "dcm group $entry, average size $dcmgroups{$entry}\n";
}
foreach my $entry (sort keys %fibergroups)
{
    print "fiber group $entry, average size $fibergroups{$entry}\n";
}
$gettables->finish();
$dbh->disconnect;

