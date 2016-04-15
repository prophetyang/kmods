# PPCDEV

An implement of char device driver

### Kernel module

##### [Build module]
```bat
shell# make 
```

##### [Clean module]
```bat
shell# make clean
```

###### [Load module]
```bat
shell# insmod ppcdev.ko
```

###### [Unload module]
```bat
shell# rmmod ppcdev
```

##### [Sysfs]
```bat
shell# ls -lah /sys/class/ppcdev/*
lrwxrwxrwx 1 root root 0  4月 15 11:09 /sys/class/ppcdev/ppcdev -> ../../devices/virtual/ppcdev/ppcdev

```

##### [Char device]
```bat
shell# ls -l /dev/ppcdev
crw------- 1 root root 247, 0  4月 15 11:09 /dev/ppcdev (where MAJOR=247, MINOR=0 here)
```

### User-space program

