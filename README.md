#Oculus Eye
####AR software for the Oculus Rift DK1 via two Playstation 3 Eye cameras

![Photo of prototype](http://i.imgur.com/MxHzK2b.jpg)

#How to use OculusEye
Controls:

| Key    |  Effect                                      |
|:------ |:-------------------------------------------- |
| C      |  Begin / end camera calibration              |
| F      |  Toggle fullscreen                           |
| + / -  |  Increase / decrease interpupillary distance |
| ESC    |  Quit                                        |

You'll find toggles at the bottom-left to open windows that reveal various settings to play with.

#Requirements
For reference, this program was developed on a mid-2012 MacBook Pro 13-inch with OSX 10.10 (Yosemite).


This program was built for Mavericks, but tested exclusively on Yosemite. Your mileage may vary greatly depending on your hardware. Bugs may present themselves on Macs with AMD GPUs. Intel GPUs work well (I know, right?) and NVidia GPUs run with minor graphical errors.

#Build Instructions
You will need to download the [OpenFrameworks 64-bit OSX branch](https://github.com/NickHardeman/openframeworks_osx_64). The project file goes in the same directory as any other OpenFrameworks project (openframeworks_osx_64/apps/myApps/). Open the XCode project and build the "OculusEye Release" scheme. The result will be placed in the /bin directory at the root folder of the project.

####Prototype footage
[![OculusEye Prototype Footage](http://img.youtube.com/vi/RTLH47_6cyU/0.jpg)](http://www.youtube.com/watch?v=RTLH47_6cyU)
___

#Credits
This project was created with [OpenFrameworks 64-bit OSX branch](https://github.com/NickHardeman/openframeworks_osx_64) using:
- [PS3EYEDriver](https://github.com/inspirit/PS3EYEDriver)
- [ofxOculusRift](https://github.com/andreasmuller/ofxOculusRift)
- [ofxTweakBar](https://github.com/roxlu/ofxTweakbar)
- [OpenCV 3.0 Beta](http://opencv.org/)
- [Intel's Threading Building Blocks](https://www.threadingbuildingblocks.org/) (4.2 Update 4)
- XCode 6 + Yosemite