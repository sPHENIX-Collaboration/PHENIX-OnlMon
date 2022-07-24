#!/bin/sh
n=0
while [ $n -lt 4 ]
do
  let "n += 1"
  for machine in `cat $ONLMON_RUNDIR/va.list`
  do
    echo $machine $n
if [[ $valgrind ]]; then
    ssh -x $machine $ONLMON_MAIN/bin/monserver.csh valgrind $n
else
    ssh -x $machine $ONLMON_MAIN/bin/monserver.csh $1 $n
fi

#  if [ $n -gt 4 ]; then
#    n=0
#  fi
  done
done
