# Connection Settings

PingViewer automatically scans the available serial (COM) ports, and communicates with the attached device to detect a Ping Echosounder. If a Ping device is detected, the connection is automatically established, and the Viewer will begin displaying data.

PingViewer also attempts to connect to UDP port 9090 on the host at IP address 192.168.2.2. This is the default IP address of the Companion software that runs on the BlueROV2. The Companion software automatically configures a communication bridge on this port to a Ping Echosounder connected to the Companion computer.