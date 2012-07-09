/*
 * Remote.java
 *
 * Copyright 2011 Thomas Buck <xythobuz@me.com>
 *
 * This file is part of xyRobot.
 *
 * xyRobot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xyRobot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xyRobot.  If not, see <http://www.gnu.org/licenses/>.
 */

import javax.swing.*;
import java.awt.*;

class SerialCommunicator {
	private String port;
	private Remote remote;

	public SerialCommunicator(String serialPort, Remote r) {
		port = serialPort;
		remote = r;
	}

	public boolean openPort() {
		if (!HelperUtility.openPort(port)) {
			remote.showError("Could not open port!");
			return false;
		} else {
			return true;
		}
	}

	public void closePort() {
		HelperUtility.closePort();
	}

	public short[] readData(int length) {
		if (port != null) {
			short[] tmp = HelperUtility.readData(length);
			if ((tmp == null) || (tmp.length != length)) {
				remote.showError("Could not read data!");
				return null;
			} else {
				return tmp;
			}
		} else {
			return null;
		}
	}

	public boolean writeData(short[] data, int length) {
		if (port != null) {
			boolean tmp = HelperUtility.writeData(data, length);
			if (!tmp) {
				remote.showError("Could not write data!");
			}
			return tmp;
		} else {
			return false;
		}
	}
}