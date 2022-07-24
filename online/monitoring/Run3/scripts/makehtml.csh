#!/bin/csh
# generate the html output from saved root file
# checks if previous process is still alive
# can be started from a cronjob (checks every 5 minutes) by 
# */5 * * * * /export/software/oncs/OnlMon/install/bin/makehtml.csh >& /dev/null
setenv prompt 1

set piddir=/tmp/phnxonlmon
set pidfile = ${piddir}/makehtml.pid
if ( ! -d $piddir ) then
  mkdir -p $piddir
  chmod a+w $piddir
endif

if ( -s $pidfile ) then
#  echo "Found Another monitor running, testing pid"
  ps eww `cat $pidfile` | grep makehtml > /dev/null 
  if ($? == 0) then
#    echo "Found Another monitor still running, exiting"
    touch $pidfile
    exit 0
  endif
endif

echo $$ > $pidfile

source /etc/csh.login
foreach i (/etc/profile.d/*.csh)
  source $i
end
source $HOME/.login
setenv DISPLAY unix:1.0
cd $ONLMON_MACROS
source setup_onlmon.csh
if ( ! -d $ONLMON_SAVEDIR/done ) the
  mkdir -p $ONLMON_SAVEDIR/done
endif
# check if we have new root files at all
ls  $ONLMON_SAVEDIR | grep Run_ | grep root

# I want a core file in case this thing crashes
#limit coredumpsize unlimited

if ( $? == 0 ) then
  foreach i ( $ONLMON_SAVEDIR/Run_*.root )
    echo $i
    root.exe -q makehtml.C\(\"$i\"\)
    mv $i $ONLMON_SAVEDIR/done
  end
else
  echo "No Root file found"
endif

rm $pidfile
exit 0
