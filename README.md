# Smart Laundry
 
An IoT project with the goal of automatically identifying the beginning and end of laundry cycles and sending notifiations (because everyone hates when they forget their clothes in the washer). It works by detecting current draw, motion or both, and currently supports notifications via Pushover, Twilio SMS and email.

## Bill of Materials

If you plan on using the included PCB files or enclosure designs, ensure that your parts match the pinouts and dimensions of the examples provided.

- Adafruit QT Py ESP32-S3 ([link](https://www.adafruit.com/product/5426)) or 30-pin ESP32 development board w/ USB-C ([example](https://www.amazon.com/dp/B0B19KRPRC))
    - The QT Py is recommended over the generic ESP32 dev board for being much smaller and having better wifi reception (in my experience). It is more expensive though, so the dev boards are a decent alternative (and still good for other projects).
- 0.96" 128x64 OLED ([example](https://www.amazon.com/gp/product/B0833PF7ML))
- 6x6x6mm side-mounting momentary button ([example](https://www.amazon.com/dp/B07JLDLV3R))
<!-- -->
- For current detection
    - Split core current transformer w/ 3.5mm connector ([example](https://www.amazon.com/gp/product/B07MJJRNSW))
        - Pick an appropriate current rating for your use case, but make sure it has a rated output of 1V
        - See note about how to use a current transformer in the [Setup](#setup) section
    - Female 3.5mm connector ([example](https://www.amazon.com/dp/B008SNZUYC))
    - ADS1115 analog-to-digital convertor ([example](https://www.amazon.com/gp/product/B07VPFLSMX))
<!-- -->
- For motion detection
    - ADXL345 accelerometer ([example](https://www.amazon.com/gp/product/B08VD7325C))

---

## Setup

1. Flash the firmware to your board and ensure that it is successfully flased. For more info on how to compile and flash the firmware code, see the [Compiling and Development](#compiling-and-development) section of the README.
1. Assemble everything on a breadboard by following the [schematic](extras/schematics/) for your board, or order a PCB using the [Gerber files](/extras/gerber/) for your board and solder your components to it.
    - You can exclude any components you opted out of.
1. Plug in your Smart Laundry's USB port to power it on. You should see a "Starting up.." message and your Smart Laundry will then start configuring itself.
    - The first startup and configuration may take longer as the filesystem has to be formatted for use.
1. Eventually you should see a "Connecting to wifi..." message. Use your smartphone or other wifi connected device to look for and connect to a network starting with "SmartLaundry_".
1. Once connected to this network, you should be automatically redirected to a configuration page. Click on "Configure WiFi", select the network you want to connect your Smart Laundry to, enter the password and hit "Save".
1. Your Smart Laundry will attempt to connect to the selected network. If it's successful, it will save the SSID and password before rebooting itself. If not, it will not reboot and you can go back and try connecting to a network again.
1. Once your Smart Laundry is connected to wifi, you should see a "Wifi connected" message and then a "No detectors enabled" message. This is because all detectors are disabled by default.
1. To configure your detectors and other settings, hold down the button on your Smart Laundry until you see a "Configuration mode" message. Then either connect to the "SmartLaundry_" network like you did before, or navigate to the IP address show on your Smart Laundry using a device connected to the same network.
1. On the Smart Laundry configuration page, click on "Setup". Here you will be shown a list of settings you can configure to your liking. All settings are explained in the [Configuration Options](#configuration-options) section of this README.
1. Once you have made your changes, click "Save" at the bottom of the page. Your Smart Laundry will save the settings and reboot. After starting up again, you should see a "Waiting for cycle" message indicating that your Smart Laundry is now reading its detectors to sense the start of a laundry cycle.
1. Mount your Smart Laundry either on or near your laundry machine, ensuring that no metal on the machine is making direct contact with the electrical components of your Smart Laundry (3D-printable enclosures coming soon&trade;).
    - Note: if you plan to use the motion detection on your Smart Laundry, you will get the best results by mounting your Smart Laundry directly to the machine to detect all vibrations
1. If you are using current detection, make sure you mount your current transformer on your machine's power cable and connect it to the 3.5mm jack on your Smart Laundry.
    - Important note about the current transformer: the current transformer will only work if it is exclusively clamped around your machine's hot wire. If it's wrapped around the entire cable, it will read always read zero. Some cords, like [this one](https://www.homedepot.com/p/GE-6-ft-3-Prong-30-Amp-Dryer-Cord-WX09X10004/203497480#overlay), have the three wires separated, so you only need to determine which line is hot and separate them enough to get the current transformer clamped on. Other cords are not as easy to separate and may require you to remove outer insulation to separate the wires. If you don't want to damage the wire that is permanently attached to your expensive laundry machine, you can get a [small 3-prong extension cord](https://www.amazon.com/dp/B075BCD1LN), cut the outer insulation to reveal the inner three wires, separate out the hot line (usually black) and then tape the other two wires back together. Please be very careful not to damage the inner wire insulation, and immediately throw away any cable where you have exposed the wiring directly. Electricity can be dangerous, so do all of this at your own risk.

---

## Configuration Options

- Device Name (default: blank)
    - Name you want to appear in notifications and in the device's access point name (e.g. "Washer" or "Dryer").
<!-- -->
- Motion Enabled? (default: un-checked)
    - Determines whether or not motion is used to detect the beginning and end of cycles.
- Motion Threshold (default: 15.0)
    - A numeric value that can be adjusted to raise or lower the bar of how much motion is required to trigger detection.
- Motion Start Duration (default: 15)
    - The number of consecutive seconds of motion above threshold required to trigger the start of a cycle.
        - Note: only one detector (motion or current) must meet the criteria to start a cycle
- Motion Stop Duration (default: 300)
    - The number of consecutive seconds of no motion above threshold being detected required to trigger the end of a cycle.
        - Note: all enabled detectors must meet the criteria to stop a cycle
<!-- -->
- Current Enabled? (default: un-checked)
    - Determines whether or not current is used to detect the beginning and end of cycles.
- Current Threshold (default: 1.0)
    - Number of amps that can be adjusted to raise or lower the bar of how much current is required to trigger detection.
- Current Start Duration (default: 0)
    - The number of consecutive seconds of current above threshold required to trigger the start of a cycle.
        - Note: only one detector (motion or current) must meet the criteria to start a cycle
- Current Stop Duration (default: 300)
    - The number of consecutive seconds of no current above threshold being detected required to trigger the end of a cycle.
        - Note: all enabled detectors must meet the criteria to stop a cycle
<!-- -->
- Startup Notification (default: checked)
    - Determines whether or not a notification is sent when the Smart Laundry turns on and connects to wifi.
- Cycle Start Notification (default: checked)
    - Determines whether or not a notification is sent at the beginning of a cycle.
- Cycle Stop Notification (default: checked)
    - Determines whether or not a notification is sent at the end of a cycle.
<!-- -->
- Pushover Enabled? (default: un-checked)
    - Determines whether notifications are sent via [the Pushover service](https://pushover.net/).
- Pushover App Token (default: blank)
    - App token provided via the Pushover web console.
- Pushover User Token (default: blank)
    - User token provided via the Pushover web console.
<!-- -->
- Twilio Enabled? (default: un-checked)
    - Determines whether notifications are sent via [the Twilio SMS platform](https://www.twilio.com/en-us/messaging/channels/sms).
- Twilio Account SID (default: blank)
    - Account SID provided via the Twilio web console.
- Twilio Auth Token (default: blank)
    - Auth token provided via the Twilio web console.
- Twilio From Number (default: blank)
    - The registered phone number notifications will be sent from, provided via the Twilio web console. The phone number should include the country code and no other special characters (e.g. +15555555555).
- Twilio To Numbers (default: blank)
    - A comma-separated list of phone numbers notifications will be sent to. The phone numbers should include the country code and no other special characters (e.g. +15555555555,+16666666666).
<!-- -->
- Email Enabled? (default: un-checked)
    - Determines whether notifications are sent via email.
- Email SMTP Host (default: blank)
    - The hostname of your email provider's SMTP server.
- Email SMTP Port (default: 0)
    - The port your email provider's SMTP server uses.
- Email SMTP Account (default: blank)
    - The email account used to sign in to your email provider's SMTP server. Also used as the from address of the email notification.
- Email SMTP Password (default: blank)
    - The password used to sign in to your email provider's SMTP server.
- Email To Addresses (default: blank)
    - A comma-separated list of email addresses notifications will be sent to (e.g. tom<span>@tom.com,jerry<span>@jerry.com).
<!-- -->
- Debug Mode (default: un-checked)
    - Enable to replace the cycle status message with sensor outputs on the screen. May be helpful for adjusting threshold values.
- Include Debug Info in Notification? (default: un-checked)
    - Enable to include extra information about sensor outputs in your cycle notifications. May be helpful for adjusting threshold values.

---

## Compiling and Development

This project is configured to use the PlatformIO ecosystem within Visual Studio Code. To build this project yourself (or even contribute to it!), you'll need to install Visual Studio Code, Python and the PlatformIO extension. [This article from Random Nerd Tutorials](https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/) does a great job explaining the process and showing you how to do it.

Once you have those tools ready, you can clone this repo and then open it with the PlatformIO extension. Then you can use the PlatformIO commands to build and upload the project to your board, as described in the article above.

If you see anything you think could be better or want to add a feature, feel free to submit a pull request!