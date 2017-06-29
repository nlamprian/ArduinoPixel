![boards](http://i76.photobucket.com/albums/j16/paign10/boards_zpso9xdzn36.jpg)

The code has been tested on Arduino Uno, Leonardo, and Mega with a WIZnet W5100 Ethernet module. Arduino Leonardo and Mega can handle at least 112 LEDs. Arduino Uno maxs out at 80+ LEDs.

API
===

The web server accepts the following HTTP requests:
* `GET` request to `/`: Responds with a `Hello from Arduino Server` message.
* `GET` request to `/strip/status`: Responds with an `ON` or `OFF` message for the on/off state of the strip.
* `GET` request to `/strip/modes`: Responds with a comma separated list of the available modes.
* `GET` request to `/strip/mode`: Responds with the name of the active mode.
* `GET` request to `/strip/color`: Responds with a JSON representation of the color of the strip, e.g. `{"r":92,"g":34,"b":127}`.
* `PUT` request to `/strip/status/on`: Turns the strip on.
* `PUT` request to `/strip/status/off`: Turns the strip off.
* `PUT` request to `/strip/mode`: Updates the mode. The required data are the name of the mode and, if applicable, a time period in ms, e.g. `SCANNER 100`.
* `PUT` request to `/strip/color`: Updates the color of the strip. The data must be formatted as a JSON object, e.g. `{"r":48,"g":254,"b":176}`.

LED Strips
==========

Currently only the NeoPixel strips are supported. If you would like to add support for a different strip, you need to extend the `LedStripBase` class, create an instance of your `LedStripX` class, and pass its pointer to the `init` method of the server.

Modes
=====

Modes exist to support dynamic effects on the strips. The available modes are SINGLE_COLOR, SCANNER, RAINBOW and RAINBOW_CYCLE. If you are interested to add your own mode, you need to extend the `ModeBase` class, and since modes are handled by the server, you also need to update the `getModes` and `updateMode` methods of `ArduinoPixelServer`.
