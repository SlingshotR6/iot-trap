# iot-trap

ESP8266 powered mouse/rat trap state detection.
Deepsleep for ~60 minutes, wakes up, compares trap state to previous.  If sprung, connects to WiFi and posts message to Slack channel.
Then deepsleep again...

To do:
- Connect to WiFi every 24 hours to check in (done).
- Add Slack user name
- Add battery voltage to daily check in (and on sprung reports)
- Wake and post immediately when trap is sprung
- Check for successful Slack post
- Set time of day for Daily Check-in
- Correct RTC drift
