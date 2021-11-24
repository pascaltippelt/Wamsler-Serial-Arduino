# Wamsler-Serial-Arduino
Use a Arduino-compatible mikrocontroller like a WEMOS D1 Mini or any other ESP module to get and set values on a Wamsler / Westminster Pellet stove.

Example: 

`/usr/bin/curl [STOVE-IP]/get?ct` will print the current ambient temperature.

`/usr/bin/curl [STOVE-IP]/set?tt=22` will set the target temperature to 22°C.

Possible to set:
tt -> target temperature
pl -> power level (0 = off)
on / off

Possible to get:
tt -> target temperature
ct -> current ambient temperature
pl -> power level
sb -> state binary (on = true, off = false)
s  -> state verbal

