Introduction
============
A small software 3D rendering program on the STM32F429 Discovery evaluation
board. Based on Andreas Mantler's work.

Build Instructions
==================
Because the STM32F429 Discovery is relatively new, we would recommend to
prepare several recent utilities:

* [OpenOCD](http://openocd.sourceforge.net/)
```
    git clone git://git.code.sf.net/p/openocd/code openocd
    cd openocd
    ./bootstrap
    ./configure --prefix=/usr/local \
        --enable-stlink
    echo -e "all:\ninstall:" > doc/Makefile
    make
    sudo make install
```
* [GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
```
    apt-get install gcc-arm-none-eabi
```
If you encounter the problems of missing packages, try to execute the
following commands in advance:
```
    sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded
    sudo apt-get update
```

In addition, you need to download [STM32F429I-Discovery firmware](http://www.st.com/web/en/catalog/tools/PF259429)
and extract it to the toplevel directory where `stm32f429-r3d` exists.

```
    unzip stsw-stm32138.zip
```

Then, get the latest source from GitHub and then build:
```
    git clone https://github.com/jserv/stm32f429-r3d
    cd stm32f429-r3d
    make
```

Please confirm STM32F429 Discovery is properly connected to USB, and let's flash the device:
```
    make flash
```

Be patient when OpenOCD is flashing.
