## Menu Items

- **Units:**
Select the units (meters or feet) for the application to display

- **Plot Theme:**
Select the color swath/gradient to use for the [Waterfall](home#waterfall) display. Custom gradients can be created according to the [User Gradients](#user-gradients) section.

- **Theme:**
Select the application menu theme (light or dark)

- **Enable Replay Menu:**
Enable the [Replay Menu](replay-data)

- **Smooth Data:**
Filter the [Waterfall](home#main-application-interface) data to smooth noise and fluctuations.

- **Antialiasing:**
Enable antialiasing in the Waterfall display rendering

- **Debug Mode:**
Enable Debug information and settings

![](images/display-settings.png)

hello

<p align="center">
    <img src="images/display-settings.png">
</p>
helllo
<p align="center">
    <img src="wiki/images/display-settings.png">
</p>

#### User Gradients

The user can create his own gradients for the waterfall, for this, it's necessary to create a file that follow some rules inside **Waterfall_Gradients** folder.

1. Filenames need to have .txt extension.
2. Filenames will be used as gradient name.
3. Filenames with underscores will be replaced with spaces.
4. Lines that do not start with # will not be processed.
5. Color values need to follow:
    1. #RGB (each of R, G, and B is a single hex digit)
    2. #RRGGBB
    3. #AARRGGBB
    4. #RRRGGGBBB
    5. #RRRRGGGGBBBB

    There is a very good tool to help with gradient colours [here](http://www.perbang.dk/rgbgradient/).
6. First value will represent 0.0
7. The last value will represent 1.0
8. The value of any other color will be `1.0*((color position) - 1)/(number of colors)`

This is an example of gradient file:
```
; Put this example in a file called `Gradient_test.txt` in **Waterfall_Gradients**.
#000000
#ffffff
#ff0000
```

Where `#000000` is white (low power signal - 0.0), `#ffffff` (average power signal - 0.5) is black and `#ff0000` is red (max power signal - 1.0).
