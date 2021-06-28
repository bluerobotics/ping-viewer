## FAQ

### System Requirements

Ping Viewer should run on any modern computer.

- Mac: OSX 10.12 or later
- Windows: Windows 10
- Linux: --

## Troubleshooting

### No Connection to the Device

- If using a UDP connection via the Blue Robotics companion computer, power on the ping device(s) before or at the same time as the companion computer, so they can be detected correctly on companion startup
- If connecting directly to a computer with a serial connection, ensure QGroundControl is closed, or that 'SiK Radio' is deselected from the 'AutoConnect to the following devices' section of [Application Settings/General](https://docs.qgroundcontrol.com/master/en/SettingsView/General.html). It's generally recommended to unselect the AutoConnect options for any device not actually being used.
![QGC AutoConnect options with SiK Radio deselected](/ping-viewer/images/external/qgc-autoconnect.png)
- Reset the ping-viewer settings by clicking the gear icon in the [application information header](application-information.md#header-buttons)
- Ensure that your usb cable is in good condition and supports data transfer. Try to use another usb cable or a cable known to work with another device like a phone.
- Ensure that the usb-uart bridge is working with your computer. The list of communication ports should change when you plug/unplug your device.
  - Windows: check the COM port list in the [windows device manager](https://en.wikipedia.org/wiki/Device_Manager)
  - Mac/Linux: check the serial devices list when you enter the command `ls /dev/tty*` in the terminal
- Ensure that the 5V, ground, RX and TX lines are connected correctly to the device. Try switching the RX and TX lines.
- Ensure that you have the necessary permissions to access the usb device. Add your user to the correct group in order to gain access to the usb device.

   - Ubuntu: `sudo usermod -a -G dialout $USER`
   - Archlinux: `sudo usermod -a -G uucp $USER`

- Ensure that you have installed the necessary drivers.
  - Windows

    If no serial device appears in Ping Viewer while connected to the computer, download and install [Windows FTDI VCP Driver Executable](https://cdn.sparkfun.com/assets/learn_tutorials/7/4/CDM21228_Setup.exe).

    1. Allow any question related to the software publisher or disk modifications.
    ![Smart Screen](https://i.imgur.com/rqY8YJA.jpg)

    2. Follow the installation procedure.
    
    [![FTDI Install Steps](https://imgur.com/r2psVwz.jpg)](https://imgur.com/r2psVwz.jpg)

    3. After finishing the installation, restart you computer and run Ping Viewer, the serial port should appear.

    ![Connection Configuration](https://i.imgur.com/yKfPuJx.jpg)

