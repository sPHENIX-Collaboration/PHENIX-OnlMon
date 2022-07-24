#!/bin/csh
#while( 1 )
#@ n=0
#while( $n < 10 )
  foreach i ( /common/a0/purschke/prdf/EVENTDATA*.PRDFF)
    echo $i
    et_putdata_direct -n 400000 $i /tmp/Monitor
  end
#  echo $n
#  @ n = $n + 1
#end
#end
