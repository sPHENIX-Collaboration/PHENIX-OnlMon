#! /bin/csh
setenv INSTALLDIR  /home/phnxtofw/test3/install
setenv ONLMON_MAIN /home/phnxtofw/test3/install
setenv LD_LIBRARY_PATH INSTALLDIR/lib:$LD_LIBRARY_PATH

if (! $?ONLMON_MAIN ) then
  echo "ONLMON_MAIN environment variable not set, exiting"
  exit
endif

# set the macros directory to the current dir (they
# reside in the same directory as this setup script)
if (! $?ONLMON_MACROS) then
  setenv ONLMON_MACROS `pwd`
endif

# create save directory if not exist
if ($?ONLMON_SAVEDIR) then
  if (! -d $ONLMON_SAVEDIR) then
    mkdir -p $ONLMON_SAVEDIR
  endif
endif

# create save directory if not exist
if ($?ONLMON_LOGDIR) then
  if (! -d $ONLMON_LOGDIR) then
    mkdir -p $ONLMON_LOGDIR
  endif
endif

if (! $?ONLMON_HTMLDIR) then
  setenv ONLMON_HTMLDIR /common/s1/htmltest/OnlMon
endif

# location to dump ert dmux files, create if not exist
if ($?ERTMASKCALIBOUT) then
  if (! -d $ERTMASKCALIBOUT) then
    mkdir -p $ERTMASKCALIBOUT
  endif
else
  setenv ERTMASKCALIBOUT ./
endif


# create history directory if not exist
if ($?ONLMON_HISTORYDIR) then
  if (! -d $ONLMON_HISTORYDIR) then
    mkdir -p $ONLMON_HISTORYDIR
  endif
endif

if (! $?ONLINE_LOG) then
  setenv ONLINE_LOG /export/data1/log
endif

if (! $?ONLINE_CONFIGURATION) then
  setenv ONLINE_CONFIGURATION /export/software/oncs/online_configuration
endif

setenv ONLMON_CALIB $ONLMON_MAIN/share
setenv ONLMON_BIN $ONLMON_MAIN/bin
setenv LD_LIBRARY_PATH $ONLMON_MAIN/lib:$OFFLINE_MAIN/lib:$LD_LIBRARY_PATH
set path = ($ONLMON_BIN $path)
# all subsystems scripts end in Setup.csh
foreach script ($ONLMON_BIN/*Setup.csh)
  source $script
end
