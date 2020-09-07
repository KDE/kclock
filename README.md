# KClock <img src="logo.png" width="40"/> 
A convergent clock application for Plasma.

## Features
* Alarms
* Stopwatch
* World Clocks
* Timers

## Links
* Project page: https://invent.kde.org/plasma-mobile/kclock
* File issues: https://invent.kde.org/plasma-mobile/kclock/-/issues
* Development channel: https://matrix.to/#/#plasmamobile:matrix.org

## Installing
This will compile and install kclock, kclockd, and the plasmoids onto the system. When running kclock, make sure that kclockd is running first (it is configured to autostart in sessions).

```
mkdir build
cd build
cmake ..
make
sudo make install
```

# Components
KClock is split into three components: **kclock** (front-end), **kclockd** (backend-daemon), and **plasmoids**.

These are individual apps and each has its own binaries. However you don't need to compile them one by one.

These apps combined together offers a powerful user experience to functionalities around Alarm, Timer and Clock.

## kclockd
The KClock daemon, which is configured to auto start on Plasma launch (located in the src folder). It has the following functions:
* Schedule wakeups with [PowerDevil](https://invent.kde.org/plasma/powerdevil), so if the system is sleeping, it will be woken up on time to ring alarms and timers.
* Exposing Alarms, Timers, and settings via D-Bus interface.
* Alarm/Timer audio and notifications, plus an indicator on system tray if there is any pending alarm.

kclockd is written with performance in mind. It does not provide any user interface by itself, as all functionalities are exposed via a D-Bus under the service name `org.kde.kclockd`. The front-end (see kclock below) uses it to add/remove and access properties of alarm/timer, and common settings like snooze minutes and ringtone volume. This allows for clock functions to continue running even when the front-end application is closed.

The plasmoids included in this repo also uses D-Bus to communicate with kclockd.

If PowerDevil is detected, it will use PowerDevil's scheduleWakeup feature to wakeup for alarm notifications. This was added in Plasma 5.20. If you want to disable this behaviour, pass the `--no-powerdevil` option when launching kclockd. Note that while running in `--no-powerdevil` mode, kclockd will fail to keep track of time if the system sleeps, which is quite common for mobile devices.

There are numerous reasons that causes kclockd will start up in `--no-powerdevil` mode automatically, including:
* Plasma 5.20 or later not being installed
* You are on a BSD system
* You choose to not to
* All of the other possiblities I forgot to mention

## KClockd D-Bus interface
After installing, five D-Bus interface XML files are copied to the KDE D-Bus interface directory. 

These are:
* org.kde.kclockd.KClockSettings.xml
* org.kde.kclockd.Alarm.xml
* org.kde.kclockd.AlarmModel.xml
* org.kde.kclockd.TimerModel.xml
* org.kde.kclockd.Timer.xml

These files can be used to generate the D-Bus adaptor. Alarm and Timer will be registered under path "/Alarms/" or "/Timers/" + its uuid's Id128 string representation (ex. *8c7d59b3befa49a48853959fe7e025d7*). However, the "remove" slots in AlarmModel or TimerModel only accept its normal representation, (ex. *{8c7d59b3-befa-49a4-8853-959fe7e025d7}*). One can get the normal string by calling the getUUID() method in each alarm/timer. Timer is in seconds precision, but you may want to call elapsed() method every 500ms or so to avoid going out of sync with kclockd. 

## kclock
A front end for kclockd written in Kirigami for Plasma Mobile and Desktop (located in src/kclock).

You can refer to this application in case you want to develop your own front end for kclockd.

Run with these environment variables to have mobile controls:
```
QT_QUICK_CONTROLS_MOBILE=true QT_QUICK_CONTROLS_STYLE=Plasma
```

## Plasmoids
Various plasmoids for kclockd and some also communicate with [KWeather](https://invent.kde.org/plasma-mobile/kweather).

Currently we have two plasmoids, expect more in the future.

Intended to provides `widgets` similar to those you can find in mainstream mobile platforms.
