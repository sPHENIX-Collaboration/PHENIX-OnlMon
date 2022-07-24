#! /bin/csh 
#

source /etc/csh.login
source $HOME/.login

whoami
sleep 1
set ETFILE=/tmp/Monitor
if ( $?ONLINE_LOG ) then
  if ( ! -d $ONLINE_LOG/OnlMon ) then
    mkdir -p $ONLINE_LOG/OnlMon
  endif
  set LOGFILE=$ONLINE_LOG/OnlMon/Monitor_etpool.log
else
  set LOGFILE=Monitor_etpool.log
endif

set MAXEVENTS=200
set DEFAULTSIZE=2000000
if ( -f $ETFILE) then
  rm -f $ETFILE
endif
if (-f $LOGFILE) then
  rm -f $LOGFILE
endif
echo "writing to $LOGFILE"
echo et_start_phenix  -f $ETFILE -s  $DEFAULTSIZE -n $MAXEVENTS  -v
et_start_phenix  -f $ETFILE -s  $DEFAULTSIZE -n $MAXEVENTS -v >>& $LOGFILE &
echo "sleeping for 5 sec to give et the chance to start up"
sleep 5
echo "done"
exit 0



