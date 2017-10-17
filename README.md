# gps-nvs
Simple arduino programs to log L1 GPS signals on a NVS NV08C-CSM.

## Hardware to use
For this project I used a [NVS NV08C-CSM](http://www.nvs-gnss.com/products/receivers/item/2-nv08c-csm.html) receiver mounted on a One Talent GNSS evaluation board called [Denga10](http://www.onetalent-gnss.com/ideas/usb-hw-receivers/denga10). Two singal streams are processed via {two arduino boards](http://wiki.seeed.cc/Seeeduino_v3.0/) and stored on SD cards through standard SD card shields. One stream directly sends a standard NMEA GPS stream onto one arduino, which uses this signal as a onboard clock as well as a trigger to make time bound files on the second arduino, which directly logs the raw L1 phase data from the NV08C.

### Arduinoe software


### wiring
to be documented
