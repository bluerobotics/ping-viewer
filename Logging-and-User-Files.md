## User Files

This section describes the user-accessible files used by the Ping Viewer application.

> All files are timestamped with the format `YMD-hmsl`, where Year(Y), Month(M), Day(D), Hour(h), Minute(m), second(s) and milisecond(l).

### Folder structure

Ping-Viewer will create 3 folders under your user documents folder.

TODO use full path

* Windows: `~/My Documents`
* Mac: `~/Documents`
* Linux: `~/Documents`

These folders are created in the documents folder

* [**Gui_Log**](#gui-logs)
* [**Sensor_Log**](#sensor-logs)
* [**Pictures**](#pictures)
* [**Waterfall_Gradients**](#waterfall-gradients)

#### GUI Logs

The GUI log files (.txt) contain debug log output from the Ping Viewer application process. All [log categories](application-information#log-categories) are captured in these logs.

#### Sensor Logs

The sensor log files (.bin) contain all of the raw binary communications received from the device. These binary communications are in Ping Protocol format. The logs can be [replayed](replay-data) by Ping Viewer application.

#### Pictures

[Screenshots](hotkeys-and-shortcuts) (.png) are stored in this folder.

#### Waterfall Gradients

[Custom gradients](display-settings#custom-gradients) (.txt) should be placed in this folder.
