#! /bin/sh
#ps uax | grep root | grep exe | awk '{print $10}'
roottime=`ps uax | grep root | grep exe | awk '{print $10}'`
#echo $roottime
if [ $roottime ]; then
  minutes=${roottime%:*}
#echo $minutes
  if [ $minutes -gt 5 ]; then
    echo Killing root running for $roottime
    date
    kill `ps uaxww | grep makehtml | grep csh | awk '{print $2}'`
    killall -9 root.exe
  fi
fi
