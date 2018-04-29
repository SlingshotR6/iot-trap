# iot-trap

ESP8266 powered mouse/rat trap state detection.
Deepsleep for ~60 minutes, wakes up, compares trap state to previous.
If sprung, connects to WiFi and posts message to Slack channel.
Then deepsleep again...

Connect IO pins, D5, D6, D7 & D1 to Ground - when these are disconnected from ground they'll report as the trap being sprung.

Connect D0 to RST - this lets the ESP8266 wake from Deepsleep

Connect battery (4.2v LiPo) to VIN & Ground.

To do:
- Connect to WiFi every 24 hours to check in.
- Add battery voltage to daily check in (and on sprung reports)
- Wake and post immediately when trap is sprung
- Check for successful Slack post
