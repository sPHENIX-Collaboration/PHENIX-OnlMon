#!/usr/local/bin/perl -w

# This script will show you the disk usage of the /a and /b disks of
# the 2 buffer boxes phnxbox4 and phnxbox5.
#
# You have to launch this one from a machine that has Tkperl installed.
# Any va is ok, but buffer boxes themselves are not.
#
# The progress bars become orange when disk usage is above 40% and red when
# above 80%
#
# 
# L. Aphecetche (aphecetc@in2p3.fr)
#
use Tk;
use Tk::FileSelect;
use Tk::ProgressBar;
use Time::localtime;

use strict;

# This is the update frequency. Don't set it too high, please.
# Default is 300 seconds (5 minutes). That should be fast enough.

my $interval = 300000; # ms

#_____________________________________________________________________________
sub diskUsage
  {
    my $machine = shift;
    my $disk = shift;

    my $cmd = "ssh -x $machine df -P $disk";

    open(HANDLE,"$cmd|") or die "Cannot df $!";
    
    my @line;

    while (<HANDLE>)
      {
	push @line,$_;
      }    

    close(HANDLE);

    my $nline = scalar @line;

    if ( $nline ne 2 ) {
# this will mark the disk as offline
	return(200);
    }

    my @result = split " ",$line[1];

    my $size = $result[1];
    my $used = $result[2];

    my $p = int $used*100/$size + 1 ;

    return ($p);
  }

#_____________________________________________________________________________

my %disks;

$disks{"phnxhome:/subsystems"} = {};
$disks{"phnxhome:/subsystems"}->{"Machine"} = "phnxhome";
$disks{"phnxhome:/subsystems"}->{"Mount"} = "/home/offline";
$disks{"phnxhome:/subsystems"}->{"Usage"} = 0;

$disks{"phnxnfs:/data"} = {};
$disks{"phnxnfs:/data"}->{"Machine"} = "phnxnfs";
$disks{"phnxnfs:/data"}->{"Mount"} = "/export/data1/log";
$disks{"phnxnfs:/data"}->{"Usage"} = 0;

$disks{"phnxnfs:/home"} = {};
$disks{"phnxnfs:/home"}->{"Machine"} = "phnxnfs";
$disks{"phnxnfs:/home"}->{"Mount"} = "/home/phnxrc";
$disks{"phnxnfs:/home"}->{"Usage"} = 0;

$disks{"phnxnfs:/software"} = {};
$disks{"phnxnfs:/software"}->{"Machine"} = "phnxnfs";
$disks{"phnxnfs:/software"}->{"Mount"} = "/export/software/oncs";
$disks{"phnxnfs:/software"}->{"Usage"} = 0;

$disks{"phnxnfs1:/ppg"} = {};
$disks{"phnxnfs1:/ppg"}->{"Machine"} = "phnxnfs1";
$disks{"phnxnfs1:/ppg"}->{"Mount"} = "/b";
$disks{"phnxnfs1:/ppg"}->{"Usage"} = 0;

$disks{"phnxnfs1:/a_scratch"} = {};
$disks{"phnxnfs1:/a_scratch"}->{"Machine"} = "phnxnfs1";
$disks{"phnxnfs1:/a_scratch"}->{"Mount"} = "/a";
$disks{"phnxnfs1:/a_scratch"}->{"Usage"} = 0;

$disks{"phnxnfs2:/standalone"} = {};
$disks{"phnxnfs2:/standalone"}->{"Machine"} = "phnxnfs2";
$disks{"phnxnfs2:/standalone"}->{"Mount"} = "/nfs2data";
$disks{"phnxnfs2:/standalone"}->{"Usage"} = 0;

$disks{"phnxnfs3:/space"} = {};
$disks{"phnxnfs3:/space"}->{"Machine"} = "phnxnfs3";
$disks{"phnxnfs3:/space"}->{"Mount"} = "/space";
$disks{"phnxnfs3:/space"}->{"Usage"} = 0;


my $mw = MainWindow->new;

my %pb;
my %usage;
my $frame1 = $mw->Frame();
$frame1->pack();
my $frame2 = $mw->Frame();
$frame2->pack();

foreach my $k ( sort keys %disks ) 
{
  my $frame = $frame1->Frame(-width=>200);
  $frame->pack(-side=>'left');

  my $title = $frame->Label(-text => $k);
  $title->pack(-side=>'top');

  $usage{$k} = $frame->Label(-text => "0%", -width=>10, -relief=>'raised');
  $usage{$k}->pack(-side=>'top');

  my $t = $pb{$k} = $frame->ProgressBar(
				     -width => 20,
				     -length => 200,
				     -anchor => 's',
				     -from => 0,
				     -to => 100,
				     -blocks => 10,
				     -gap => 0,
				     -colors => [0, 'green', 40, 'orange' , 80, 'red'],
				    );
  $t->pack(-side=>'top');
}

my $quitButton = $mw->Label(-text => "Quit", -relief => 'raised');
$quitButton->pack(-after=>$frame2,-fill=>'x',-expand=>'both',-side=>'top');
$quitButton->bind('<Button-1>' => sub {$mw->destroy});

my $date = $mw->Label(-text => "", -relief => 'sunk');
$date->pack(-after=>$frame2,-fill=>'x',-expand=>'both',-side=>'bottom');


#_____________________________________________________________________________

$mw->repeat($interval,\&update);

&update;

MainLoop;

#_____________________________________________________________________________
sub update()
  {
    foreach my $d ( keys %disks ) 
      {
	my $u = diskUsage($disks{$d}->{"Machine"},$disks{$d}->{"Mount"});
	$disks{$d}->{"Usage"} = $u;
	$pb{$d}->value($u);
        if ($u <= 150)
          {
if ($u > 100)
{
    $u = 100;
}
	    $usage{$d}->configure(-text => $u."%");
          }
        else
          {
	    $usage{$d}->configure(-text => "offline");
          }
	$date->configure(-text => ctime());
      }
  }
