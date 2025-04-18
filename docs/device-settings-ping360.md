![Ping360 Settings](images/viewer/ping360-settings.png)

## Menu Items

- **Range:** adjust the distance from the sonar to scan signals. Smaller ranges will scan faster as the receiver does not have to wait as long to receive a response. Changes to this setting will automatically adjust the transmit duration (advanced setting) for best performance in most applications.
- **Receiver Gain:** Adjusts receiver sensitivity, choices are **low**, **medium** and **high**. Higher gain is more likely to detect smaller/further targets, but is also more likely to saturate the signal for closer/larger targets. Where the signal saturates it clips to the maximum intensity, so there is less detail for determining precise distances, or differentiating between multiple strong responses. Saturation is mostly problematic for mapping with/analysis of the scan data, and is less of an issue for navigation.
- **Sector Angle**: Choose full 360 scan, or select a smaller area to scan directly in front of the sonar. Smaller sector angles take less time to scan. For non-full sectors the scan angle bounces back and forth within the sector boundaries.
- **Reset Settings:** Reset the settings in this panel to their default configuration.

#### Advanced Settings

> Click the arrow on the Device Settings menu to access these advanced settings

- **Transmit Duration:** The length of time to transmit for in each ping. Longer durations send more sound energy, so are more likely to get a response, but come at the cost of range precision for the responses. Updates to the range setting automatically update this to maximise response strength for the current range resolution. In enclosed or otherwise acoustically noisy environments it can help to reduce this and the receiver gain, and in open water with far away and/or small targets it can help to increase them.
- **Transmit Frequency:** Changes the frequency of the emitted pulse. [Lower frequencies are less absorbed by water](https://resource.npl.co.uk/acoustics/techguides/seaabsorption/physics.html), but the Ping360 transducer is tuned to 750kHz and less sensitive to other frequencies. Practically recommended to stay within 650-850kHz. Can make minor changes to account for the doppler effect if consistently moving quickly (e.g. if moving at 1.5m/s (BlueROV2 max forward speed) reduce by 1-3kHz to optimise receiver sensitivity to objects being approached). May be useful for reducing interference to other nearby sensors with a similar acoustic range sensitivity.
- **Speed of Sound:** The speed of sound to be used for distance calculations. At 20℃ this should be roughly 1520 m/s in salt water, 1480 m/s in fresh water (It's not possible to use Ping360 in air!). See [this calculator](https://bluerobotics.com/learn/speed-of-sound-in-water-calculator) for more accurate speeds in water at different depths, temperatures, and salinity levels.
- **Angular Resolution:** Changes the step between each sample. Enables scanning a sector faster at the expense of a lower angular resolution.
