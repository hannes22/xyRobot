/*
 * Remote.java
 *
 * Copyright 2012 Thomas Buck <xythobuz@me.com>
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
	private Remote remote;
	private boolean opened = false;

	public final int ERROR = -1;
	public final int TIMEOUT = -2;
	public int lastError = 0;

	public SerialCommunicator(Remote r) {
		remote = r;
	}

	public boolean openPort(String port) {
		if (!HelperUtility.openPort(port)) {
			return false;
		} else {
			opened = true;
			return true;
		}
	}

	public void closePort() {
		HelperUtility.closePort();
		opened = false;
	}

	public boolean isOpen() {
		return opened;
	}

	public String readLine() {
		// Read until \n, return as String, with \n stripped
		if (opened) {
			StringBuilder ret = new StringBuilder();
			while(true) {
				short[] data = readData(1);
				if ((data == null) || (data.length) != 1) {
					return null;
				}
				char c = (char)data[0];
				if (c != '\n') {
					ret.append(c);
				} else {
					return ret.toString();
				}
			}
		} else {
			return null;
		}
	}

	public boolean writeChar(int c) {
		int errorCount = 10;
		short[] dat = new short[1];
		dat[0] = (short)c;
		while (errorCount > 0) {
			if (writeData(dat, 1)) {
				return true;
			}
		}
		return false;
	}

	public short[] readData(int length) {
		if (opened) {
			short[] tmp = HelperUtility.readData(length);
			if (tmp == null) {
				lastError = TIMEOUT;
				return null;
			} else if (tmp.length != length) {
				lastError = ERROR;
				return null;
			} else {
				return tmp;
			}
		} else {
			return null;
		}
	}

	private boolean writeData(short[] data, int length) {
		if (opened) {
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