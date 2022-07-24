#! /bin/csh 
#
# this removes unused stations so they don't pile up and at some point block 
# new stations from attaching
#*/17 * * * *  /home/phnxrc/online_monitoring/scripts/et/et_remove_unused_stations.csh >& /dev/null

source /etc/csh.login
source $HOME/.login

et_remove_station /tmp/Monitor >& $HOME/et_remove_station.log
