QSoftAP - A QT based virtual WLAN utility for Windows 7 and above
========================================================

What does it do?
================

- Set up virtual WLAN(wireless local network), by which you can share resources (like files, internet connection) with your mobile or other PCs
- Manage shared folders easily
- QT 5.15.2 & 6.2.3 build successfully
- msvc 2019 & qt creater tested

Screenshot
==========

![](./screen_shot.png)

How to use it?
==============

- Download the [binary package](./release.7z) and unpack it
- Install `vcredist_x86.exe`(vc++ 2013 runtime)
- Run `QSoftAP.exe` as <font color='red'> administrator </font>

How to build the source code?
=============================

- MSVC++ 12.0 compiler (Visual Studio 2013) installed
- Qt5 SDK configured correctly
- If you don't use Visual Studio, then you have to tell the linker to include these libraries
  - Iphlpapi.lib
  - Netapi32.lib
  - Wlanapi.lib
  - Ws2_32.lib
- For debugging, you have to **run your IDE as administrator**

What it can't do?
=============================
- permit/allow wifi client
- statistics of client traffic
