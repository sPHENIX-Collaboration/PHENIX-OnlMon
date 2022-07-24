#! /usr/bin/perl
# change the menu and menu.html files,
# remove entries which don't point to plots

use Env;
use File::Copy;

if (!$ONLMON_HTMLDIR)
{
    die "setup script not sourced, ONLMON_HTMLDIR is not set\n";
}

print "html dir: $ONLMON_HTMLDIR\n";

open(F,"find $ONLMON_HTMLDIR/*data -name 'menu*' |");
while($file = <F>)
{
    chomp $file;
#    print "$file\n";
    $filestr = $file;
    $dirname = $file;
    $dirname =~ s/\/menu//g;
    $dirname =~ s/\.html//g;
    @sp1 = split(/\//,$filestr);
    $runnumber = $sp1[$#sp1-1];
#    print "run $runnumber, dir: $dirname\n";
    if ($file =~ /\.html/)
    {
	$menuhtml = 1;
    }
    else
    {
	$menuhtml = 0;
    }
    open(F1,$file);
    open(F2,">menu.tmp");
    {
	while($line = <F1>)
	{
	    chomp $line;
	    if ($menuhtml == 0)
	    {
		@pngfile = split(/\//,$line);

		$fullfile = $dirname . "/" . $pngfile[$#pngfile];
	    }
	    else
	    {
		@pngfile = split(/\"/,$line);
		if ($#pngfile > 0)
		{
#		    print "$line\n";
#		    print "png: $pngfile[1]\n";
		    $fullfile = $dirname . "/" . $pngfile[1];
		}
		else
		{
		    print F2 "$line\n";
		    next;
		}
	    }
	    if (-e $fullfile)
	    {
		print F2 "$line\n";
#		    print "found $fullfile\n";
	    }
	    else
	    {
#		    print "$fullfile not found\n";
	    }


	}
    }
    close(F1);
    close(F2);
#    $newname = "bck." . $file;
#    move($file,$newname);
    $status = move("menu.tmp",$file);
    if ($status == 0)
    {
	die "move(\"menu.tmp\",$file) failed\n";
    }
}
close (F);

