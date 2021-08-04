![Display Settings](/ping-viewer/images/viewer/display-settings.png)

## Menu Items

- **Units:**
Select the units (meters or feet) for the application to display

- **Theme:**
Select the application menu theme (light or dark)

- **Enable Replay Menu:**
Enable the [Replay Menu](replay-data.md)

- **Head Down (Ping360 only):**
Project the polar scan as though [viewing the seabed](https://bluerobotics.com/learn/understanding-and-using-scanning-sonars/#viewing-the-seabed). 

- **Flip A-Scan**:
Flip between furthers/closest ping data at the top of the [Return Plot](index.md#return-plot).

- **Smooth Data:**
Filter the [Waterfall](index.md#waterfall) data to smooth noise and fluctuations.

- **Antialiasing:**
Enable antialiasing in the Waterfall display rendering

- **A-Scan:**
Enable a display of the signal intensity of the latest ping ([Return Plot](index.md#return-plot)).

- **Plot Theme:**
Select the color swath/gradient to use for the [Waterfall](index.md#waterfall) display. Custom gradients can be created according to the [custom gradients](#custom-gradients) section.

- **Heading Integration (Ping360 only):**
Integrate with ROV compass reading to allow sonar display to account for ROV rotations.

- **Debug Mode:**
Enable Debug information and settings

#### Custom Gradients

The user can create his own gradients for the waterfall, for this, it's necessary to create a file that follow some rules inside **Waterfall_Gradients** folder.

1. Filenames need to have .txt extension.
2. Filenames will be used as gradient name.
3. Filenames with underscores will be replaced with spaces.
4. Lines that do not start with # will not be processed.
5. Color values need to follow:
    1. `#RGB` (each of R, G, and B is a single hex digit)
    2. `#RRGGBB`
    3. `#AARRGGBB`
    4. `#RRRGGGBBB`
    5. `#RRRRGGGGBBBB`

    There is a very good tool to help with gradient colours [here](http://www.perbang.dk/rgbgradient/).
6. First value will represent 0.0
7. The last value will represent 1.0
8. The value of any other color will be `1.0*((color position) - 1)/(number of colors)`

This is an example of gradient file:
```
; Put this example in a file called `Gradient_test.txt` in **Waterfall_Gradients**.
#7f000000
#ffffff
#ff0000
```

Where `#7f000000` is black with 50% transparency (low power signal - 0.0), `#ffffff` (average power signal - 0.5) is white and `#ff0000` is red (max power signal - 1.0).
