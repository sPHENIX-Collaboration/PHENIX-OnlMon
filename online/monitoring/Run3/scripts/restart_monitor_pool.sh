#! /bin/bash
#
# this script restarts the 
# online monitoring et pool

echo "removing monitoring et pools"
ssh -x etpool "/etc/stop_et.sh"
echo "starting monitoring et pools"
ssh -x etpool "/etc/start_et.csh"


