![Firmware Update](/ping-viewer/images/viewer/firmware-update.png)

The Firmware Update menu allows you to update the programming on a Ping device.

## Automatic Firmware Update

Steps to flash the device:

1. [Connect](device-manager.md) to the device
2. Open the Firmware Update menu
3. Choose 'Automatic Update', and the baudrate that you would like to use to communicate with the device (115kbps or 9600bps, if you are not sure choose 115kbps)
4. Click _FIRMWARE UPDATE_ to start the process.
5. The update progress is indicated by the progress bar located at the bottom of the menu. When the update completes, the progress bar will resume it's idle animation, and the Ping device will resume data transmission.

> Wait for the update process to complete before unplugging the device!

![Firmware Update Waiting](/ping-viewer/images/viewer/firmware-update-waiting.gif)

## Manual Firmware Update

Steps to flash the device:

1. [Connect](device-manager.md) to the device
2. Open the Firmware Update menu
3. Choose 'Manual Update'
4. Download your desired firmware from the `/ping1d` folder inside the [firmware repository](https://github.com/bluerobotics/ping-firmware/tree/master/ping1d)
    1. Right click 'Raw' and click 'Save Link As...' to save the firmware hex file (`.hex`)
    ![Raw](/ping-viewer/images/firmware-update/download.png)
    2. If the file format is coming up as 'Text Document (.txt)' or similar, change it to 'HEX File (.hex)' if possible, or select 'All files' and ensure the file extension is `.hex`, not `.hex.txt`
5. In Ping-Viewer, click `Browse` to select the firmware hex file that you saved
6. Click _FIRMWARE UPDATE_ to start the process.
7. The update progress is indicated by the progress bar located at the bottom of the menu. When the update completes, the progress bar will resume it's idle animation, and the Ping device will resume data transmission.

> Wait for the update process to complete before unplugging the device!

![Manual Firmware Update](/ping-viewer/images/firmware-update/manual.png)


[//]: # (Can we add a subsection here about updating through BR companion computer? This would likely be helpful for anyone who doesn't want to take apart their ROV but wants to update the firmware)

## Device recovery

If the device is not working after flashing a new firmware, or if some problem was encountered during installation, a recovery process may be necessary.

The device must be opened to restore the firmware. We only want to open the device if it's absolutely necessary, so to make sure, please [reset ping-viewer settings](https://docs.bluerobotics.com/ping-viewer/application-information/#header-buttons), and connect the device to the computer again. If ping-viewer detects the device automatically, there is no problem! If not, then we can proceed to recover the device.

To open the device, hold it firmly, and turn the blue ring in counterclockwise direction:

![Firmware Update Waiting](/ping-viewer/images/firmware-update/open-ping.png)

After the device is opened, turn it on and look for a blinking led on the circuit board inside of the device. If you see a blinking led, the device should be good and the firmware is running fine, you may close the device and double check the [troubleshooting instructions](http://docs.bluerobotics.com/ping-viewer/faq-and-troubleshooting/#troubleshooting).

If the led is blinking:

 - No recovery is necessary, the firmware is working fine
 - Try with a different serial-USB adapter
 - Check if the adapter is providing 5V and all connections are correct without short circuits
 - Contact Blue Robotics support email

If the led is not blinking:

 - Open the folder where the ping-viewer executable binary is located
 - Copy the folder path (we will use `/folder/path` in this example)
 - Open your OS terminal. (powershell on windows, what you prefer on linux :) )
 - type: `cd "/folder/path"`
 - After that you should download the last firmware available:
   - On windows: `Invoke-WebRequest -Uri "https://raw.githubusercontent.com/bluerobotics/ping-firmware/master/ping1d/Ping_V3.26_115kb.hex" -OutFile "$PWD/Ping_V3.26_115kb.hex"`
   - On mac/linux: `wget "https://raw.githubusercontent.com/bluerobotics/ping-firmware/master/ping1d/Ping_V3.26_115kb.hex"`
 - With the device open, you should see a BOOT button in the main board. Power down the device, press and hold this button down, then power the device and let go of the button
 - You should check the port of the device with windows **Device Manager** or with `dmesg` on linux.

 ![Firmware Update Waiting](/ping-viewer/images/firmware-update/device-manager.png)

 - After finding the port (`COMx` on windows and `/dev/ttyUSB*` on linux) you can start the flash procedure.
 - In the same terminal type:
   - On windows `.\stm32flash.exe -v -g 0x0 -b 115200 -w .\Ping_V3.26_115kb.hex COM4`
     - Where COM4 is my serial port
   - On linux `.\stm32flash -v -g 0x0 -b 115200 -w .\Ping_V3.26_115kb.hex /dev/ttyUSB0`
     - Where /dev/ttyUSB0 is my serial port

You should see the following output:

```
stm32flash 0.5

http://stm32flash.sourceforge.net/

Using Parser : Intel HEX
Interface serial_posix: 115200 8E1
Version      : 0x31
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0446 (STM32F302xD(E)/F303xD(E)/F398xx)
- RAM        : 64KiB  (6144b reserved by bootloader)
- Flash      : 512KiB (size first sector: 2x2048)
- Option RAM : 16b
- System RAM : 8KiB
Write to memory
Erasing memory
Wrote and verified address 0x080202c8 (100.00%) Done.

Starting execution at address 0x08000000... done.
```

Keep in mind that the writing percentage should be 100%, if not, check your connections.

If the instructions are not clear or sound a bit complicated, please wait a bit more for us to create a Ping-Viewer version that allows you to recover devices.
