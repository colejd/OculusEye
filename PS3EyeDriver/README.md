#PS3EyePlugin
PS3EyePlugin is an efficient dylib for accessing Playstation 3 Eye cameras over USB. It has both a C++ and Objective-C interface, allowing it to work with either language. Additionally, it includes an interface for Unity, should you wish to use it there.

Currently, this library is configured for 64-bit; however, 32-bit or universal is not out of the question. You'll need to rebuild Boost with universal support.

#Requirements
- OSX 10.9+
- LibUSB (1.09)
    - libboost_system-mt.a
    - libboost_thread-mt.a
- Boost (1.57)
    - libusb-1.0.a

You may install these with Homebrew:

```
brew install libusb
brew install boost
```

#Credits
This plugin is based on [PS3EYEDriver](https://github.com/inspirit/PS3EYEDriver) by inspirit.

#License
This plugin is licensed under the MIT License. See [the license](LICENSE.txt) for more.

#Known Bugs
For whatever reason, if you load this plugin in Unity, the Boost thread that updates the camera hardware takes right around 60 seconds to kick into gear. To be frank, I have no idea why. It seems to be an issue with Unity itself.


Made by [Jonathan Cole](www.joncole.me)