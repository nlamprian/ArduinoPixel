Command Line
============

Apart from the ArduinoPixel Android app, you can control your LED strip from your linux command line with the `arduino_pixel` client.

To make the tool accessible from any directory, it's recommended to copy the file in `~/bin` and make it executable with the following command:

```
chmod u+x ~/bin/arduino_pixel
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
