Command Line
============

Apart from the ArduinoPixel Android app, you can control your LED strip from your linux command line with the `arduino_pixel` client.

To make the tool accessible from any directory, you should copy the `cmd_line/arduino_pixel` file to `/usr/bin` and make it executable with the following command:

```
chmod +x /usr/bin/arduino_pixel
```

To avoid specifying the server URI every time you use arduino_pixel, you can set it up as an environment variable. Assuming that you are using the bash shell, carefully type the following command (you'll have to substitute your own server ip and port):

```
echo "export ARDUINO_PIXEL_URI=<ip>:<port>" >> ~/.bashrc
```

The above command will define the ARDUINO_PIXEL_URI variable at the end of your .bashrc file.

Now, you can use the client like so:

```
arduino_pixel put /strip/status/on  # Turn the strip on
arduino_pixel put /strip/mode 'RAINBOW_CYCLE 10'  # Enable the RAINBOW_CYCLE mode with 10ms period
```

Service
=======

Most often I turn on my LED strip when I sit on my desk at night. In other words, I turn on the strip when I start my computer and turn it off when I shut the computer down. But why do this manually, when the computer can handle it automatically? That's why I wrote the `arduino_pixel_service` script. Inside the script, you get to define a schedule for the operation of the strip. That is, you must specify a time of the day that the strip should turn on and a time that it should turn off. If you run the script, it will keep track of time and power the strip as necessary.

Firstly, you need to set up the ARDUINO_PIXEL_URI environment variable. The entry in your .bashrc will not do in this case. So, copy `env/arduino_pixel_service` to `/etc/default`.

Secondly, you need to place the script in a directory that's in your PATH. Just copy `script/arduino_pixel_service` to `/usr/bin` and make it executable with the following command:

```
chmod +x /usr/bin/arduino_pixel_service
```

The script is written in Python. If you are missing any of the required modules, use `pip` to install them.

Lastly, you need to set up a daemon to run the script when you start your computer. Copy `daemon/arduino_pixel_service` to `/etc/init.d` and execute the following commands:

```
chmod 755 /etc/init.d/arduino_pixel_service
update-rc.d arduino_pixel_service defaults
update-rc.d arduino_pixel_service enable
```

Now, the script will execute when you start your computer, and depending on the current time, it will either wait or immediately turn the strip on, until the time comes to turn it off, and so on.
