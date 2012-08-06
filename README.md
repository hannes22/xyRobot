# xyRobot

xyRobot is the Framework for my Robot. [Visit my website for more informations](http://xythobuz.org/index.php?p=rob).

## Software used

 + [Peter Fleurys TWI Library](http://homepage.hispeed.ch/peterfleury/avr-software.html)
 + [Linux Version of Peter Danneggers 'fastboot' by H. C. Zimmerer](http://www.mikrocontroller.net/topic/146638)
 + [A Linux compatible Upload Software for fastboot](http://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_type=project&item_id=1927)

## xyRobotRemote

xyRobotRemote is a Java App which communicates with the Robot Firmware over a serial port. It allows you to shoot pictures with the Gameboy camera and remote-control the robot. It requires Java 6 and could possibly work with Java 7...

## Protocol

The protocol used by the xyRobot Firmware and xyRobotRemote to remote-control the robot is as follows.

<table>
<tr><th>Character Sequence</th><th>Action</th></tr>
<tr><td>ASCII '?'</td><td>Print Firmware Version String</td></tr>
<tr><td>0x80, x</td><td>Rotate the camera Up/Down to the position indicated by the next byte, x, (0 to 180)</td></tr>
<tr><td>0x81, x</td><td>Same as 0x80, but Left/Right</td></tr>
<tr><td>0x82, r1...r8</td><td>Send picture. Camera Register Settings set with the following 8 bytes.</td></tr>
<tr><td>0x83, d, s, dir</td><td>Drive dist cm with speed speed. Direction Forwards if dir = 1, Backwards if dir = 0</td></tr>
<tr><td>0x84, deg, dir</td><td>Rotate deg Degree. Direction Right if dir = 1, ;eft if dir = 0</td></tr>
<tr><td>0x85, r1...r8</td><td>Send picture with 4bit per Pixel.</td></tr>
<tr><td>0x86</td><td>Returns measured distance.</td></tr>
<tr><td>0x87, r1...r8</td><td>Send picture with 2bit per Pixel.</td></tr>
<tr><td>0x88, r1...r8</td><td>Send picture with 1bit per Pixel.</td></tr>
</table>

New major releases could change this protocol or make additions.
