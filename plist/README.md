This module lists all processes per second

[Load module]
shell# insmod plist.ko

[Unload module]
shell# rmmod plist

[Result]
dmesg

[Example]
[  648.333019] [1] ppid=0 command=init, utime=172000000, stime=1192000000, gtime=0
[  648.333022] [2] ppid=0 command=kthreadd, utime=0, stime=4000000, gtime=0
[  648.333025] [3] ppid=2 command=ksoftirqd/0, utime=0, stime=16000000, gtime=0
[  648.333028] [4] ppid=2 command=kworker/0:0, utime=0, stime=0, gtime=0
[  648.333031] [5] ppid=2 command=kworker/0:0H, utime=0, stime=0, gtime=0
[  648.333034] [6] ppid=2 command=kworker/u4:0, utime=0, stime=128000000, gtime=0
[  648.333036] [7] ppid=2 command=rcu_sched, utime=0, stime=8000000, gtime=0
[  648.333039] [8] ppid=2 command=rcuos/0, utime=0, stime=28000000, gtime=0
[  648.333042] [9] ppid=2 command=rcuos/1, utime=0, stime=16000000, gtime=0
[  648.333045] [10] ppid=2 command=rcu_bh, utime=0, stime=0, gtime=0
[  648.333048] [11] ppid=2 command=rcuob/0, utime=0, stime=0, gtime=0
[  648.333054] [12] ppid=2 command=rcuob/1, utime=0, stime=0, gtime=0
[  648.333070] [13] ppid=2 command=migration/0, utime=0, stime=52000000, gtime=0
[  648.333073] [14] ppid=2 command=watchdog/0, utime=0, stime=0, gtime=0
[  648.333076] [15] ppid=2 command=watchdog/1, utime=0, stime=0, gtime=0
[  648.333087] [16] ppid=2 command=migration/1, utime=0, stime=52000000, gtime=0
....
[  648.333340] plist_task exits
[  648.333360] Cleaning up module.

