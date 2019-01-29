## FAQ

### System Requirements

Ping Viewer should run on any modern computer.

- Mac: OSX 10.12 or later
- Windows: Windows 10
- Linux: --

## Troubleshooting

### No Connection to the Device

- Ensure that you have [installed the necessary drivers](home#installing-and-running-the-application)
- Ensure that you have the [necessary permissions to access the usb device. Add your user to the correct group in order to gain access to the usb device.

   - Ubuntu: `sudo usermod -a -G dialout $USER`
   - Archlinux: `sudo usermod -a -G uucp $USER`

- Ensure that your usb cable is in good condition and supports data transfer. Try to use another usb cable or a cable known to work with another device like a phone
