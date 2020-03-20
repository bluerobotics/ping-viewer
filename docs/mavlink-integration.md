## MAVLink Integration

Vehicles that run *ArduPilot* (E.g: **ArduSub** and *ArduRover*) communicate using a protocol called **[MAVLink](https://mavlink.io/en/)**,
Some supported **Ping Viewer** sensors can communicate with these vehicles to fetch the necessary information to improve
the visualization experience and provide extra information about the environment and perspective.

### MAVLink Source

Ping Viewer by default expects to communicate with a vehicle that contains a companion computer, or any other component
that can provide MAVLink messages via an UDP server with the IP address **192.168.2.2** and port **14660**.

> Note: The UDP server IP address and port can't be configured with the actual version of Ping-Viewer.


### Supported Devices and Visualization Settings

Right now Ping360 is the only supported device that integrates with MAVLink, the visualizer updates the heading based on
vehicle orientation to improve pilot perspective about the environment.

<video style="width:90%; margin: auto; display: block;" autoplay loop controls="controls">
  <source type="video/webm" src="https://s3.amazonaws.com/downloads.bluerobotics.com/videos/ping360-pingviewer.mp4"></source>
  <p>Your browser does not support the video element.</p>
</video>
