## FAQ

### System Requirements

Ping Viewer should run on any modern computer.

- Mac: OSX 10.12 or later
- Windows: Windows 10
- Linux: --

## Troubleshooting

### No Connection to the Device

- Reset the ping-viewer setting by clicking the gear icon in the [application information header](https://docs.bluerobotics.com/ping-viewer/application-information/#header-buttons)
- Ensure that your usb cable is in good condition and supports data transfer. Try to use another usb cable or a cable known to work with another device like a phone.

- Ensure that you have the necessary permissions to access the usb device. Add your user to the correct group in order to gain access to the usb device.

   - Ubuntu: `sudo usermod -a -G dialout $USER`
   - Archlinux: `sudo usermod -a -G uucp $USER`

- Ensure that you have installed the necessary drivers.
  - Windows

    If no serial device appears in Ping Viewer while connected to the computer, download and install [Windows FTDI VCP Driver Executable](https://cdn.sparkfun.com/assets/learn_tutorials/7/4/CDM21228_Setup.exe).

    1. Allow any question related to the software publisher or disk modifications.
    <p align="center">
    <img src="https://i.imgur.com/rqY8YJA.jpg">
    </p>

    2. Follow the installation procedure.
    <p align="center">
    <a href="http://i.imgur.com/r2psVwz.jpg">
        <img src="https://imgur.com/r2psVwz.jpg">
    </a>
    </p>

    3. After finishing the installation, restart you computer and run Ping Viewer, the serial port should appear.

    <p align="center">
    <img src="https://i.imgur.com/yKfPuJx.jpg">
    </p>
