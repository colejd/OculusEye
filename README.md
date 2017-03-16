# OculusEye
Made by [Jonathan Cole](www.joncole.me)

OculusEye is an open-source wearable augmented reality platform with a focus on cost-effectiveness. The software portion of this project is based on OpenCV 3.0. The hardware is comprised of cheaply available parts: two Playstation 3 Eye cameras, and a [modular, 3d-printable mount](/etc/models).

The main focus of the software portion of this project was the creation of an efficient dylib for using Playstation Eye cameras, and a program utilizing this through an AR-focused rendering pipeline via OpenCV.

![Photo of prototype](http://i.imgur.com/MxHzK2b.jpg)
[![OculusEye Prototype Footage](http://img.youtube.com/vi/RTLH47_6cyU/0.jpg)](http://www.youtube.com/watch?v=RTLH47_6cyU)

# How to use OculusEye
Controls:

| Key    |  Effect                                      |
|:------ |:-------------------------------------------- |
| E      |  Toggle edge detection                       |
| R      |  Toggle Oculus Distortion                    |
| F      |  Toggle fullscreen                           |
| S      |  Swap camera sides                           |
| G      |  Toggle stereo GUI                           |
| N      |  Show / hide nose                            |
| + / -  |  Increase / decrease interpupillary distance |
| C      |  Begin / end camera calibration              |
| ESC    |  Quit                                        |

You'll find toggles at the bottom-left to open windows that reveal various settings to play with.

# Requirements
For reference, this program was developed on a mid-2012 MacBook Pro 13-inch with OSX 10.10 (Yosemite).

This program was built for Mavericks, but tested exclusively on Yosemite. Your mileage may vary greatly depending on your hardware. Bugs may present themselves on Macs with AMD GPUs. Intel GPUs work well (right?) and NVidia GPUs run with minor graphical errors.

# Build Instructions
You will need to download the [OpenFrameworks 64-bit OSX branch](https://github.com/NickHardeman/openframeworks_osx_64). The project file goes in the same directory as any other OpenFrameworks project (openframeworks_osx_64/apps/myApps/). Open the XCode project and build the "OculusEye Release" scheme. The result will be placed in the /bin directory at the root folder of the project.

Everything in the /data directory will be copied into /bin/data.

___

# License
This program is licensed under the MIT License. See [the license](LICENSE.txt) for more.

# Credits
This project was created with [OpenFrameworks 64-bit OSX branch](https://github.com/NickHardeman/openframeworks_osx_64) using:
- [ofxOculusRift](https://github.com/andreasmuller/ofxOculusRift)
- [ofxTweakBar](https://github.com/roxlu/ofxTweakbar)
- [OpenCV 3.0 Beta](http://opencv.org/)
- [Intel's Threading Building Blocks](https://www.threadingbuildingblocks.org/) (4.2 Update 4)
- XCode 6 + Yosemite
