#!/usr/bin/perl -w

###  from confuration file PBSC reference list
### 
### read_pbsc_conf_ch72.pl < PBSC.conf >PBSC.dat
###
while(<>)
{
  chomp($line=$_);
  @words=split(/ /,$line);  

  if($line=~/^fem:/i && $line=~/no/i )
  {
    foreach  (@words)
    {
      if(/^pos=/i) 
      {	  
	s/\D+//;
	s/,+//;		
	$pos=$_;
#	print "******* New FEM ************ $pos \n";
      }
    }
  }
  
 #if($line=~/^ref:/i && $line=~/type=1/i)
  if($line=~/^ref:/i) 
  { 

    my ($pos_sm) = 0;
    my ($sector) = 6;
    my ($type) = 0;   
     
    foreach  (@words)
    {
        
      if(/^type=/i)
      {
        s/\D+//;
        s/,+//;
        $type = $_ - 1;
      } 
      if(/^pos=/i)
      {
        s/\D+//;
        s/,+//;
        $pos_sm = $_;    
      }
      if(/^sector=/i)
      {
        s/,+//;
        if($_ eq "sector=w0")
        {
           $sector=0;     
        } 
        if($_ eq "sector=w1")
        {
           $sector=1;
        }    
        if($_ eq "sector=w2")
        {  
           $sector=2; 
        }
        if($_ eq "sector=w3")
        {
           $sector=3;
        }  
        if($_ eq "sector=e2")
	{
           $sector=4;
        } 
        if($_ eq "sector=e3")
        {
           $sector=5;
        }   
               
      }

      if(/^chan=/i) 
      {	  
	s/\D+//;
	s/,+//;		
	$chan=$_; 
	# convert 144 to 72 channels
        $delta = 0; 
	if($chan >=96 && $chan < 120)
	{
	  $delta = 48;
	}  
	if($chan >=48 && $chan < 72)
	{
	   $delta = 24; 
	}
	$id=$pos*144+$chan - $delta;
        $index=$sector*18+ $pos_sm + $type*108;
	#print "$pos $chan $id \n";
	#print "$id \n";
        print "$id $index\n";
      }
    }
  }

}







