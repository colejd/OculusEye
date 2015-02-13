AR software for the Oculus Rift DK1 via two Playstation 3 Eye cameras.

![Photo of prototype](http://i.imgur.com/MxHzK2b.jpg)

#How to use OculusEye
Controls:

| Key |  Effect             |
|-----|:-------------------:|
| C   |  Calibrate camera   |
| F   |  Toggle fullscreen  |
| ESC |  Quit               |

#Build Instructions
You will need to download the [OpenFrameworks 64-bit OSX branch](https://github.com/NickHardeman/openframeworks_osx_64). The project file goes in the same directory as any other OpenFrameworks project (openframeworks_osx_64/apps/myApps/). Open the XCode project and build the "OculusEye Release" scheme. The result will be placed in the /bin directory at the root folder of the project.

Please note that this program currently does not work with AMD CPUs or GPUs.

#Credits
Created with [OpenFrameworks 64-bit OSX branch](https://github.com/NickHardeman/openframeworks_osx_64) using:
- [PS3EYEDriver](https://github.com/inspirit/PS3EYEDriver)
- [ofxOculusRift](https://github.com/andreasmuller/ofxOculusRift)
- [ofxTweakBar](https://github.com/roxlu/ofxTweakbar)
- [OpenCV 3.0 Beta](http://opencv.org/)
- [Intel's Threading Building Blocks](https://www.threadingbuildingblocks.org/) (4.2 Update 4)
- XCode 6 + Yosemite