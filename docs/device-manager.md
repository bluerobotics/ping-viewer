PingViewer automatically scans for available devices on serial ports (COM) and UDP connections. If a Ping device is detected, the available connection is shown and the user will be able to choose between all available devices in **Device Manager**.

PingViewer attempts to connect to UDP ports 9090 ([Ping1D](https://bluerobotics.com/store/sensors-sonars-cameras/sonar/ping-sonar-r2-rp/)) and 9092 ([Ping360](https://bluerobotics.com/store/sensors-sonars-cameras/sonar/ping-sonar-r2-rp/)) on the host at IP address 192.168.2.2. This is the default IP address of the **Companion computer** that runs on the BlueROV2. The **Companion computer** automatically configures a communication bridge on these ports to a connected sensor.

#### Device Manager

![Device Manager](/ping-viewer/images/viewer/device-manager-1.png)

    1- Shows an animation and the name of the available sensor.
    2- Describes the kind of the available connection.
    3- Shows the availability of the connection.
        **Red**: The connection is not available/reliable.
        **Green**: The sensor is connected and available for connection.
    4- Describes the connection type and the connection configuration.
    5- Allows the user to go to **Manual Connection**.
    6- Cancel and exit **Device Manager**.

#### Manual Connection

![Manual Connection](/ping-viewer/images/viewer/device-manager-2.png)

    1- Displays all devices compatible.
    2- Shows available connection types.
    3- Lists all available serial ports.
    4- Displays the valid baud rates for the selected device.
        > Note: If the device uses **Automatic Baud Rate detection**, the baud rate list will not appear.
    5- Connects with the configured settings.
    6- Goes back to **Device Manager** menu.
    7- Cancel and exit **Manual Connection**.

## Connection Types

- **Serial:** Select the serial port (eg. `COM1` or `ttyUSB0`) and baudrate, that will be updated by the possible baud rates for the sensor.
- **UDP:** Select the host UDP server IP address and port.

> Note: To connect the application to a Ping device on a remote host (eg. a Raspberry Pi) you can use the pingproxy program in the bluerobotics-ping python package:
>
> `pip install --user bluerobotics-ping`
>
> `python -m brping/pingproxy --device <device>`
>
> `<device>` is the serial port where the Ping device is connected (eg. `/dev/ttyUSB0`).
