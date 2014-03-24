ArduinoPixel
============

This project consists of two pieces. The first piece is an **Arduino sketch** that implements a **Web Server** and offers an **API** for controlling a [NeoPixel LED Strip](http://www.adafruit.com/products/1138). The second piece is an **Android app**, `ArduinoPixel`, that connects to the **Arduino Web Server** and sends **commands** to control the **color** and the **on/off state** of the **LED strip**.

<br>
![promo](http://i859.photobucket.com/albums/ab154/lampnick67/promo__zpsbbabfe87.png)
<br><br>

The **Arduino sketch** is also available at [codebender](https://codebender.cc/sketch:31742). You can **clone** the project, **update** the controller and network parameters, and **upload** it straight to your **Arduino Ethernet**, or any other Arduino compatible board w/ an Ethernet Shield.

The **Android application** is available on [Google Play](https://play.google.com/store/apps/details?id=ln.paign10.arduinopixel). **Install** the app to your phone or tablet, **configure** the network parameters you set earlier in the Arduino sketch, and you are **ready** to go. You can watch a demo on [YouTube](http://www.youtube.com/watch?v=AuqOQ0Pe_c0).

API
---

The server accepts the following HTTP requests:
* A `GET` request to `/`. It responds with a **Hello from Arduino Server** message.
* A `GET` request to `/strip/status/`. It responds with **ON** or **OFF** for the on/off state of the strip.
* A `GET` request to `/strip/color/`. It responds with a **JSON representation** of the strip's **color**, e.g. `{"r":92,"g":34,"b":127}`.
* A `PUT` request to `/strip/status/on`. It **turns** the strip **on**.
* A `PUT` request to `/strip/status/off`. It **turns** the strip **off**.
* A `PUT` request to `/strip/color/`. It **changes** the strip's **color**. The **data** are delivered as a **JSON object**, e.g. `{"r":48,"g":254,"b":176}`.


Testing
-------

The **Android app** was tested on a tablet with Android v`4.1.1`.


Attribution
-----------

The **Android app** makes use of **Piotr Adamus'** [ColorPicker](https://github.com/chiralcode/Android-Color-Picker) view.

**Thank you Piotr**... with your **beautiful** work, you gave me the motivation I needed to attempt this project.