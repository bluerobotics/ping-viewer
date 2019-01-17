TODO update image after layout rework
<p align="center">
    <img src="images/ping-settings.png">
</p>

## Menu Items

- **Emit Ping:** Request a single profile sample from the device, and stop continuous transmission of profile data (if active).
- **Ping Rate Slider:** Enable continuous transmission of profile data, and set the maximum update rate (the actual update rate may be slower).
- **Speed of Sound:** The speed of sound to be used for distance calculations. This should be 1500 m/s in salt water, 1435 m/s in fresh water, and 343 m/s in air (for testing!).
- **Ping Enabled:** Enable the acoustic output of the Ping device. When set to disabled, the Ping will not emit energy from the transducer. When set to enabled, energy may be emitted for background operations, even when there is no data being transmitted.

#### Advanced Settings

Click the wrench on the Device Settings menu to access these advanced settings

- **Auto Gain:** Enable automatic gain and range adjustments (ENABLED is the recommended default settings).
- **Gain Selection:** Manual gain control (automatic mode must be disabled). Adjust the sensitivity of the device; larger numbers indicate greater sensitivity.
- **Scan Range:** Manual scan range selection (automatic mode must be disabled). Manually select the scan range for data output and target tracking. There are two parameters to adjust the scan range:
 - *Scan Start*: The distance away from the Ping device transducer to begin the scan
 - *Scan Length*: The total distance to be scanned (the distance from *Scan Start* to end the scan)
