This is the repo for KClocd, KClock and plasmoids. These are individual apps and each has its own binaries. However you don't need to compile them one by one.

Each app has its own roles and none of them is a substitute to another.

These apps combined together offers a powerful user experience to functionalities around Alarm, Timer and Clock.

# KClockd
KClock daemon, auto start on plasma launch. It does following things:
* Schedule wakeup with PowerDevil, so if your system is sleeping, we can still wakeup on time to ring alarm.
* Expose Alarms, Timers, and settings via D-Bus interface.
* Alarm/Timer audio and notifications, plus an indicator on system tray if there is any pending alarm.

KClockd is written with performance in mind. Itself doesn't provide any user interface, all functionalities are exposed via D-Bus under service name `org.kde.kclockd`, you can add/remove and access properties of alarm/timer. And common settings like snooze minutes and ringtone volume. For actuall front end implementation you can refer to KClock.

The plasmoids included in this repo also uses D-Bus to communicate with KClockd.

If PowerDevil is detected, it will use PowerDevil's scheduleWakeup feature to wakeup for alarm notifications. This was added in Plasma 5.20. If you want to disable this behaviour, pass the `--no-powerdevil` option when launching KClockd. Note that if running in `--no-powerdevil` mode, KClockd will fail to keep tracking of time if system sleeps. Which is quite common for mobile devices.

There are numerous reasons that causes KClockd running in `--no-powerdevil` mode, include:
* Plasma 5.20 or later not installed
* You are on BSD system
* You choose to not to
* All of the other possiblities I forgot to mention

## KClockd D-Bus interface
TODO

# KClock
A front end for KClockd written in Kirigami for Plasma Mobile and Desktop.

You can refer to this app in case you want to develop your own front end for KClockd.

## Features
* Alarms
* Stopwatch
* World Clocks
* Timer

Run with these environment variables to have mobile controls:
```
QT_QUICK_CONTROLS_MOBILE=true QT_QUICK_CONTROLS_STYLE=Plasma
```
# Plasmoids
Various plasmoids for KClockd and some also communicate with KWeather.

Currently we have two plasmoids, expect more in the future.

Intended to provides `widgets` simlilar to those you can find in mainstream mobile platforms.
