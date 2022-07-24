#! /bin/sh
# this script starts the virtual frame buffer on display 1:0. To be used
# for background root jobs to send their graphical output, so we can make
# gif files
# can be started from a cronjob (checks every 5 minutes) by 
# */5 * * * * /export/software/oncs/OnlMon/install/bin/xvfb.sh start >& /dev/null
# or add this to the inittab:
#S2:345:respawn:/usr/bin/Xvfb -ac -nolisten tcp -fbdir /tmp -screen 0 1024x768x16 :1
#set -x

piddir=/tmp/phnxonlmon
pidfile=$piddir/xvfb.pid

[ -d $piddir ] || mkdir -p $piddir
chmod a+w $piddir

#tmpwatch deletes untouched files after some time in /tmp
#touching them should prevent this
if [ -f /tmp/Xvfb_screen0 ]; then
  touch /tmp/Xvfb_screen0
fi

if [ -s $pidfile ]; then
  touch $pidfile
fi

case "$1" in
  start)
    if [ -f /tmp/Xvfb_screen0 ]; then
# echo Xvfb already running
       if [ -f $pidfile ]; then
          ps `cat $piddir/xvfb.pid` |grep Xvfb  >/dev/null
          if [ $? = 0 ]; then
	    exit
          fi
       fi
    fi
    /usr/X11R6/bin/Xvfb -ac  -nolisten tcp -fbdir /tmp -screen 0 1024x768x16 :1 >& /dev/null &
    pid=`echo $!`
    echo $pid > $pidfile
    chmod a+w $pidfile
    sleep 10s
    /usr/X11R6/bin/xset s off -display :1.0
  ;;
  stop)
    if [ -f $pidfile ]; then
      kill `cat $pidfile`
      if [ $? != 0 ]; then
        killall Xvfb
      fi
      rm $pidfile
    else
      killall Xvfb
    fi
  ;;
  restart)
    $0 stop
    $0 start
  ;;
  *)
    echo $"Usage: $0 {start|stop|restart}"
    exit 1
  ;;
esac

exit 0
