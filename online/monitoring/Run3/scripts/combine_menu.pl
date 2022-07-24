#!/usr/local/bin/perl

use strict;
use warnings;
use Getopt::Long;
use File::Copy;
use File::Basename;

my $writefiles;
GetOptions("write"=>\$writefiles);

if ($#ARGV < 1)
{
    print "usage: combine_menu.pl <dir1> <dir2 (and outdir)>\n";
    print "flags:\n";
    print "--write : write ouput menu files\n";
    exit(-2);
}

my $dir1 = $ARGV[0];
my $dir2 = $ARGV[1];

if (! -d $dir1)
{
    print "input directory $dir1 not found\n";
    exit(-3);
}
if (! -d $dir2)
{
    print "input directory $dir2 not found\n";
    exit(-3);
}

my $menu1 = sprintf("%s/menu",$dir1);
my $menu2 = sprintf("%s/menu",$dir2);
my $outmenu = sprintf("%s/outmenu",$dir2);

if (! -f $menu1)
{
    print "could not find menu file $menu1\n";
    exit(-4);
}
if (! -f $menu2)
{
    print "could not find menu file $menu2\n";
    exit(-4);
}

my %subsystems = ();
my %subsystemsfound = ();
open(F,"$menu1");
while(my $line = <F>)
{
    chomp $line;
    my @sp1 = split(/\//,$line);
    if (! exists $subsystems{$sp1[0]})
    {
	my $array = [];
	$subsystems{$sp1[0]} = $array;
	$subsystemsfound{$sp1[0]} = 1;
    }
    push(@{$subsystems{$sp1[0]}},$line);

}
close(F);

open(F,"$menu2");
while(my $line = <F>)
{
    chomp $line;
    my @sp1 = split(/\//,$line);
    if (! exists $subsystems{$sp1[0]})
    {
	my $array = [];
	$subsystems{$sp1[0]} = $array;
	if (exists $subsystemsfound{$sp1[0]})
	{
	    print "duplicate entries found, subsystem $sp1[0]\n";
	    exit(-5);
	}

    }
    push(@{$subsystems{$sp1[0]}},$line);

}
close(F);


if (defined $writefiles)
{
    open(F,">$outmenu");
    foreach my $k (sort keys %subsystems) 
    { 
	foreach (@{$subsystems{$k}}) 
	{ 
	    print F "$_\n"; 
	}  
    }
    close(F);
    move($outmenu,$menu2);
}

$menu1 = sprintf("%s/menu.html",$dir1);
$menu2 = sprintf("%s/menu.html",$dir2);
my $htmloutmenu = sprintf("%s/outmenu.html",$dir2);

if (! -f $menu1)
{
    print "could not find menu file $menu1\n";
    exit(-4);
}
if (! -f $menu2)
{
    print "could not find menu file $menu2\n";
    exit(-4);
}

my %htmlsubsystems = ();
my %htmlsubsystemsfound = ();
my $hashkey;
open(F,"$menu1");
while (my $line = <F>)
{
    chomp $line;
    if ($line =~ /\<HR/)
    {
	next;
    }
    if ($line =~ /\<H(\d+)/)
    {
	my $hval = $1;
	if ($hval == 1)
	{
	    if (! exists $htmlsubsystems{$line})
	    {
		my $array = [];
		$htmlsubsystems{$line} = $array;
		$htmlsubsystemsfound{$line} = 1;
		push(@{$htmlsubsystems{$line}},"<HR><BR>");
		$hashkey = $line;
	    }
	}

    }
    push(@{$htmlsubsystems{$hashkey}},$line);
}
close(F);

open(F,"$menu2");
while (my $line = <F>)
{
    chomp $line;
    if ($line =~ /\<HR/)
    {
	next;
    }
    if ($line =~ /\<H(\d+)/)
    {
	my $hval = $1;
	if ($hval == 1)
	{
	    if (! exists $htmlsubsystems{$line})
	    {
		my $array = [];
		$htmlsubsystems{$line} = $array;
		if (defined $htmlsubsystemsfound{$line})
		{
		    print "duplicated entries for subsystem $line\n";
		    exit(-5);
		}
		$htmlsubsystemsfound{$line} = 1;
		push(@{$htmlsubsystems{$line}},"<HR><BR>");
		$hashkey = $line;
	    }
	}

    }
    push(@{$htmlsubsystems{$hashkey}},$line);
}
close(F);

if (defined $writefiles)
{
    open(F,">$htmloutmenu");
    foreach my $k (sort keys %htmlsubsystems) 
    { 
	foreach (@{$htmlsubsystems{$k}}) 
	{ 
	    print F "$_\n"; 
	}  
    }
    close(F);
    move($htmloutmenu,$menu2);
}

my @copyfiles = <$dir1/*>;
foreach my $file (@copyfiles) 
{
    my $lfn = basename($file);
    if ($lfn eq "menu" || $lfn eq "menu.html")
    {
	next;
    }
    my $newfile = sprintf("%s/%s",$dir2,$lfn);
    if (-f $newfile)
    {
	print "file $lfn exists also in target dir\n";
	exit(-6);
    }
    else
    {
	if (defined $writefiles)
	{
	    move($file,$dir2);
	}
	else
	{
	    print "would move $file to $dir2\n";
	}

    }
}
