#!/usr/bin/perl -w

###
###  from confuration file PBGL reference list
### 
### read_pbgl_conf.pl < pbgl_tmp.conf >PBBG.dat
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
  
#  if($line=~/^ref:/i && $line=~/type=1/i)
  if($line=~/^ref:/i )
  { 

    my ($pos_sm) = 0;
    my ($sector) = 0;
    my ($type)   = 0;
    foreach  (@words)
    { 

      if(/^type=/i)
      {
        s/\D+//;
        s/,+//;
        $type = $_ -3;                             
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
         if($_ eq "sector=e1")
         { 
            $sector = 1;                 
         }  
         else {
            $sector = 0; 
         }       
      }
      
      if(/^chan=/i) 
      {	  
	s/\D+//;
	s/,+//;		
	$chan=$_; 
	$id=($pos-4)*144+$chan;
       # $i0 = (($type % 2)? 72 : 0);
       # $i0 = $type;
        $index = $sector*192 + $type + $pos_sm*6; 
       #($type % 2)*64;  
	#print "$pos $chan $id \n";
        print "$id  $index\n";
      }
    }
  }

}













