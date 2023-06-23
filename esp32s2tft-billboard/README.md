Adafruit ESP32S2-TFT Billboard
==============================

![](https://github.com/domschl/python-dwd-forecast/blob/master/resources/esp32-billboard.jpg)

A simple example demonstrating muwerk's scheduler, munet interface to WLAN, 
with a TFT display that shows weather information downloaded from a [web server](https://github.com/domschl/python-dwd-forecast).


T.B.D.

## Building with platformio

* In data directory, copy `net-default.json` to `net.json`
* Customize `net.json` and `billboard.json`
* Customize `src/ca_root.h` and insert your own root certificate (alternatively set `"insecure_ssl": true` below).
* Build filesystem: `pio run -t buildfs`
* Upload filesystem: `pio run -t uploadfs`
* Upload program code: `pio run -t upload`

## Configuration

Three files need to be customized:

- Within the `data` directory, copy `net-default.json` to `net.json` and enter your network credentials into `net.json`.
- Then customize the file `billboard.json`: 

```json
{
    "server": "nalanda",
    "port": 8289,
    "url": "/ministation/10865",
    "insecure_ssl": false,
    "refresh_interval_secs": 600
}
```

The billboard esp32 will try to access a server at ```https://nalanda/ministation/10865``` using SSL. The validity of the server's certificates is checked with the root ca authority from `src/ca_root.h`, if not disabled (via `"insecure_ssl": true`).

- For secure SSL (default), you need to alter the file `ca_cert.h` and insert the root ca authority that can be used to identify your server's certificates. If you want to skip this, set `"insecure_ssl": true`, the certificate chain then will not be checked.

The url above needs to send a bitmap of size 240x135 encoded in RGB565 (16bit per pixel).

### Image format RGB565

The following code (in Python) shows how to scale and convert an arbitrary PNG image into the required RGB565 format:

```python
from PIL import Image
import numpy as np

def img2rgb565(self, png_image_file, output_file):
        image = Image.open(png_image_file).convert("RGB")
        image_rgb = image.resize((240, 135))
        # Convert the image to a numpy array
        image_array = np.array(image_rgb)
        # Extract the individual color channels
        red = image_array[:, :, 0].astype(np.uint16)
        green = image_array[:, :, 1].astype(np.uint16)
        blue = image_array[:, :, 2].astype(np.uint16)
        # Convert the color channels to 5-bit and 6-bit precision
        red = np.bitwise_and(red >> 3, 0x1F).astype(np.uint16)
        green = np.bitwise_and(green >> 2, 0x3F).astype(np.uint16)
        blue = np.bitwise_and(blue >> 3, 0x1F).astype(np.uint16)
        # Combine the color channels into a single 16-bit array
        rgb565 = np.bitwise_or(
            np.bitwise_or(red << 11, green << 5).astype(np.uint16), blue
        ).astype(np.uint16)
        # Flatten the array
        rgb565_flat = rgb565.flatten()
        # Convert the array to binary data
        binary_data = rgb565.tobytes()  # rgb565_flat.astype(np.uint16).tobytes()
        # Save the binary data to a file
        with open(output_file, "wb") as file:
            file.write(binary_data)
```

See the project [python_dwd_forecast](https://github.com/domschl/python-dwd-forecast/tree/master) for a complete secure web server example in python.

## Documentation

* [muwerk scheduler and messaging](https://github.com/muwerk/muwerk)
* [munet networking and mqtt](https://github.com/muwerk/munet)
* [documentation of hardware mupplets and supported MQTT messages](https://github.com/muwerk/mupplet-core)
* See [DWD weather server](https://github.com/domschl/python-dwd-forecast) for a server example with secure SSL.
