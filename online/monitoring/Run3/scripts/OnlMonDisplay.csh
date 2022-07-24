#!/bin/csh
# crontab entry:
#*/10 * * * * /export/software/oncs/OnlMon/install/bin/OnlMonDisplay.csh start >& /dev/null

setenv prompt 1

if ($#argv != 1) then
 echo "Usage: $0 {start|stop|restart}"
 exit 1
endif

set hostname=`hostname`
if ($hostname != 'phnxdisplay0.phenix.bnl.gov') then
  echo "Run this script only on phnxdisplay0"
  exit 2
endif

set piddir=/tmp/phnxonlmon
set pidfile = ${piddir}/onlmondisplay.pid
if ( ! -d $piddir ) then
  mkdir -p $piddir
  chmod a+w $piddir
endif

if ( -f $pidfile ) then
  touch $pidfile
endif

#check if root job is running
switch ("$1")
  case "start":
    if ( -f $pidfile ) then
       ps `cat $pidfile` |grep root.exe  >/dev/null
       if ( ! $? ) then
	  exit 0
       endif
    endif
#    killall -9 root.exe
    source /etc/csh.login
    foreach i (/etc/profile.d/*.csh)
      source $i
    end
    source $HOME/.login
    cd $ONLMON_MACROS
    source $ONLMON_MACROS/setup_onlmon.csh
    setenv DISPLAY phnxdisplay0:0.3
# I want a core file in case this thing crashes
#  limit coredumpsize unlimited
# phnxdisplay0 has a 10 base T and it takes forever to write
# a GB sized corefile so lets not do this for the time being
#    limit coredumpsize 0
    nohup root.exe -l $ONLMON_RUNDIR/overhead.cmd >& /dev/null &
    set pid=`echo $!`
    echo $pid > $pidfile
    chmod a+w $pidfile
    breaksw
  case "status":
    if ( -s $pidfile ) then
      ps `cat $pidfile`
      if ( $? != 0 ) then
        echo "Overhead display Dead"
        rm $pidfile
      endif
    else
        echo "No Overhead Display Running"
    endif
  breaksw
  case "stop":
    if ( -s $pidfile ) then
      kill -9 `cat $pidfile`
      if ( $? != 0 ) then
        echo "No Overhead Display process " `cat $pidfile`
      else
        echo "Killed  Overhead Display process " `cat $pidfile`
      endif
      rm $pidfile
    endif
  breaksw
  case "restart":
    $0 stop
    $0 start
  breaksw
  case "*"
    echo "Usage: $0 {start|stop|restart|}"
    exit 1
  breaksw
endsw

exit 0

