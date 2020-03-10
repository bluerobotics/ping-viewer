# Binary log file structure

The log file that Ping Viewer provides has a header description structure that allows program to identify different types of information, the header structure is defined as the following:

- Header (**String**)
- Log structure version (**int32**)
- Information about the program that have generated the log
  - Ping Viewer commit hash (**String**)
  - Ping Viewer build date (**String**)
  - Ping Viewer release tag (**String**)
  - Operating System name (**String**)
  - Operating System version (**String**)
- Sensor information
  - Sensor type (**int32**)
  - Sensor model (**int32**)

> Note: The binary file structure is done in **big-endian**.

Arrays (**Strings**) are defined with the following structure:
- Array size (**uint32**)
- Array data (**byte_array[]**)

The source code that defines this structure can be found in Ping Viewer source code [LogSensorStruct file](https://github.com/bluerobotics/ping-viewer/blob/master/src/link/logsensorstruct.cpp).

## Python example

You can check [this minimal Python example](https://github.com/bluerobotics/ping-viewer/blob/master/examples/decode_sensor_binary_log.py) (needs Python 3.7+).

> It's necessary to install `bluerobotics-ping` python package before running it.

It outputs the **Header** content, and after an user input, it decodes some specific messages from the file.
E.g:
```
Header:
    String: PingViewer sensor log file
    Version: 1
    PingViewerBuildInfo:
        hash: b0461486
        date: 2020-01-03T14:31:14-03:00
        tag: development
        os:
            name: Arch Linux
            version: unknown
    Sensor:
        Family: 1
        Type: 2

Press Enter to continue and decode received messages...
```
