#! /bin/sh
#
# Source function library.
. /etc/rc.d/init.d/functions
echo    

echo -n $"killproc et_monitor"
killproc et_monitor 
echo

echo -n $"killproc et_start_phenix"
killproc et_start_phenix
echo

rm -f /tmp/Monitor
rm -f /tmp/Monitor_temp*

# this is only neccessary if one runs the etlogger
#for shm in `ipcs | grep phnxrc | awk '{print $2}'`; do
#    
#  ipcrm shm $shm
#done

exit 0
