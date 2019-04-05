PingViewer automatically scans the available serial (COM) ports, and communicates with the attached device to detect a Ping Echosounder. If a Ping device is detected, the connection is automatically established, and the Viewer will begin displaying data.

PingViewer also attempts to connect to UDP port 9090 on the host at IP address 192.168.2.2. This is the default IP address of the Companion software that runs on the BlueROV2. The Companion software automatically configures a communication bridge on this port to a Ping Echosounder connected to the Companion computer.

The Ping device connection can also be configured explicitly in the Connection Settings menu:

TODO take new shot without sonar type
![Connection Settings](/ping-viewer/images/viewer/connection-settings.png)

## Connection Types

- **Serial:** Select the serial port (eg. `COM1` or `ttyUSB0`) and baudrate (this should be 115200) to use for the device connection
- **UDP:** Select the host UDP server IP address and port

    To connect the application to a Ping device on a remote host (eg. a Raspberry Pi) you can use the pingproxy program in the bluerobotics-ping python package:

    ```sh
    sudo pip install bluerobotics-ping
    python -m brping/pingproxy --device <device>
    ```

    `<device>` is the serial port where the Ping device is connected (eg. `/dev/ttyUSB0`).
