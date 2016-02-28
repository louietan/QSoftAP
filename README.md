QSoftAP
=======

Main features
=============

- Virtual WLAN configuration
- Internet connection sharing
- Folder sharing management

**The portable executable binaries are packed in [Release.7z](./Release.7z)**

Screenshot
==========

![](./screen_shot.png)

How to build
============

This project is developed using Visual Studio 2013 with Qt5 Add-in. For other build systems, you need to add the link commands for the following import libraries.
- Iphlpapi.lib
- Netapi32.lib
- Wlanapi.lib
- Ws2_32.lib
