# kclock
A clock application in Kirigami for Plasma Mobile and desktop.

This application continues to run in the background when the window is closed.

If PowerDevil is detected, it will use PowerDevil's scheduleWakeup feature to wakeup for alarm notifications. This was added in Plasma 5.20. If you want to disable this behaviour, pass the `--no-powerdevil` option when launching kclock.

## Features
* Alarms
* Stopwatch
* World Clocks
* Timer

Run with these environment variables to have mobile controls:
```
QT_QUICK_CONTROLS_MOBILE=true QT_QUICK_CONTROLS_STYLE=Plasma
```
