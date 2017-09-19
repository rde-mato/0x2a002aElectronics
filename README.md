# 0x2a002aElectronics
42 Elec Project - MIDI Sequencer

The 0x2a002a is a MIDI sequencer project. It was part of a school program at 42 Paris (http://www.42.fr/) where most students have limited to none prior experience with electronics, both Firmware and Hardware.

The project spans on a 6 month period, and was carried by Remi de Matos, Théophile Vallée and Laurent Meyer. Each team choses their own project, and makes it completely from scratch with only one hour of class per week:
* definition of technical specs: user manual, capabilities, parts, consumption.
* design of the electric schemas and routing of the PCB (using Altium software)
* building, soldering, and 3D printing for some parts
* writing asynchronous firmware, without using any external libraries except for the C standard library (including SPI, I2C, UART and chip-specific drivers)
* the only requirement is to use a PIC32 microcontroler.

The result of our project is a fully functional MIDI controler, capable of managing 16 instruments, with each of them 16 patterns of 16 beats.

Watch how Remi is happy about it: https://www.facebook.com/laurent.meyer/posts/10159690303240355
