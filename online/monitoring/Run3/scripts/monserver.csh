#!/bin/csh
# start the background monitoring and check if it is still
# alive
# can be started from a cronjob (checks every 5 minutes) by 
# */5 * * * * /export/software/oncs/OnlMon/install/bin/monserver.csh >& /dev/null

if ($#argv != 2) then
 echo "Usage: $0 {start|stop|restart|valgrind} n (1-5)"
 exit 1
endif

@ n=$2
if ($n < 1 || $n > 5) then
  echo "Number of Monitor between 1 and 5"
  exit 1
endif

set piddir=/tmp/phnxonlmon
set pidfile = ${piddir}/onlmon$2.pid

if ( ! -d /data2/onlmon) then
  mkdir -p /data2/onlmon
endif

if ( ! -d $piddir ) then
  mkdir -p $piddir
  chmod a+w $piddir
endif

if ( -f $pidfile ) then
  touch $pidfile
endif

#check if root job is running
switch ("$1")
  case "valgrind":
    $0 stop $2
    setenv valgrind on
    $0 start $2
  breaksw
  case "start":
    if ( -s $pidfile ) then
       ps `cat $pidfile` |grep root.exe  >/dev/null
       if ( ! $? ) then
	  exit 0
       endif
    endif
    source /etc/csh.login
    foreach i (/etc/profile.d/*.csh)
      source $i
    end
    source $HOME/.login
    cd $ONLMON_RUNDIR
    source $ONLMON_MACROS/setup_onlmon.csh
    setenv DISPLAY unix:1.0
    set cmdfile=`hostname`.$2.cmd
    if ( ! -f $cmdfile ) then
      echo cannot find command file $ONLMON_RUNDIR/$cmdfile
      exit 1
    endif
    set logfile = /data2/onlmon/`hostname`.$2.log
    if ( -f $logfile ) then
      set bkglog = $logfile.`date +%m-%d-%Y:%H-%M-%S`
      mv $logfile $bkglog
    endif
# I want a core file in case this thing crashes
    limit coredumpsize unlimited
    if ($?valgrind) then
      set valoutdir = $HOME/onlmon_valgrindlog
      if (! -d $valoutdir) then
        mkdir -p $valoutdir
      endif
      nohup valgrind -v --num-callers=40 --error-limit=no --suppressions=$ROOTSYS/root.supp --leak-check=full --leak-resolution=high --log-file=$valoutdir/`hostname`.$2.vallog root.exe -l `hostname`.$2.cmd >& /data2/onlmon/`hostname`.$2.log &
    else
      nohup root.exe -l `hostname`.$2.cmd >& /data2/onlmon/`hostname`.$2.log &
    endif
    set pid=`echo $!`
    echo $pid > $pidfile
#    sleep 10
#    chmod g+w /data2/onlmon/`hostname`.$2.log
    chmod a+w $pidfile
    breaksw
  case "status":
    if ( -s $pidfile ) then
      ps `cat $pidfile`
      if ( $? != 0 ) then
        echo "Monitoring Dead"
        rm $pidfile
      endif
    else
        echo "No Monitoring Running"
    endif
  breaksw
  case "killall":
    echo "Doing mass kill of all root processes"
    killall -9 root.exe
    foreach i ($piddir/onlmon*.pid)
      rm $i
    end
  breaksw
  case "stop":
    if ( -s $pidfile ) then
      kill -9 `cat $pidfile`
      if ( $? != 0 ) then
        echo "No Online Monitoring process " `cat $pidfile`
      else
        echo "Killed Online Monitoring process " `cat $pidfile`
      endif
      rm $pidfile
    endif
  breaksw
  case "restart":
    $0 stop $2
    $0 start $2
  breaksw
  case "*"
    echo "Usage: $0 {start|stop|restart|valgrind|killall (Caution: kills ALL ROOT JOBS on Node)} n (1-5)"
    exit 1
  breaksw
endsw

exit 0
