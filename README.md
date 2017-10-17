# gps-nvs
Simple Arduino programs to log L1 GPS signals on a NVS NV08C-CSM. This is non maintained research code, so you will have to dig through the in-line comments with imagination to understand what is going on. Enjoy.

## Hardware to use
For this project I used a [NVS NV08C-CSM](http://www.nvs-gnss.com/products/receivers/item/2-nv08c-csm.html) receiver mounted on a One Talent GNSS evaluation board called [Denga10](http://www.onetalent-gnss.com/ideas/usb-hw-receivers/denga10). Two signal streams are processed via two [Arduino boards](http://wiki.seeed.cc/Seeeduino_v3.0/) and stored on SD cards through standard SD card shields. One stream directly sends a standard NMEA GPS stream onto one Arduino, which uses this signal as a onboard clock as well as a trigger to make time bound files on the second Arduino, which directly logs the raw L1 phase data from the NV08C.

### Arduino software
Two Arduino programs exist. Log_NV08C_raw initializes the NVS board via a HEX stream through UART and then starts to happily log L! phase raw data onto the SD card until it gets a trigger pulse from the second Arduino, by which it creates a new raw file on the SD card.
The second program Log_NV08C_NMEA sets up a custom NMEA string, reads that string to process time as well as writes the GPS coordinates produced by the NVS board to the SD card. It also logs an optional [accelerometer](http://wiki.seeed.cc/Grove-3-Axis_Digital_Accelerometer-16g/) to monitor the GPS antenna tilt during observation. When a certain time has passed, this Arduino sends a trigger signal to the raw logging Arduino to change file names on the raw stream.

Which Arduino libraries you need you can see in the program headers.

### wiring
to be documented
