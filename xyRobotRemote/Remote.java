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
import java.awt.event.*;

public class Remote extends JFrame implements KeyListener {

	private final int width = 400;
	private final int height = 400;

	private String serialPort = null;
	private PaintCanvas canvas = null;

	public Remote() {
		super("xyRobotRemote");
		setSize(width, height);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		addKeyListener(this);
		setLayout(null);
		Container c = getContentPane();

		canvas = new PaintCanvas(256, 256, 2);
		canvas.setBounds(10, 10, 256, 256);
		c.add(canvas);
		canvas.randomize();
		canvas.addKeyListener(this);
		
		setVisible(true);
	}

	public Remote(String port) {
		this();
		serialPort = port;
	}

	public void keyPressed(KeyEvent e) { }

	public void keyReleased(KeyEvent e) {
		switch (e.getKeyChar()) {
			case 'r':
				canvas.randomize();
				break;

			case 'q':
				System.exit(0);
				break;
		}
	}

	public void keyTyped(KeyEvent e) { }

	private short[] readData(int length) {
		if (serialPort == null) {
			serialPort = askForSerialPort(HelperUtility.getPorts());
			if (serialPort == null) { showError("Could not ask for port!"); System.exit(1); }
			return readData(length);
		} else {
			if (HelperUtility.openPort(serialPort)) {
				short[] tmp = HelperUtility.readData(length);
				HelperUtility.closePort();
				return tmp;
			} else {
				showError("Could not open port!");
				System.exit(1);
			}
		}
		return null;
	}

	private void writeData(short[] data, int length) {
		if (serialPort == null) {
			serialPort = askForSerialPort(HelperUtility.getPorts());
			if (serialPort == null) { showError("Could not ask for port!"); System.exit(1); }
			writeData(data, length);
		} else {
			if (HelperUtility.openPort(serialPort)) {
				boolean tmp = HelperUtility.writeData(data, length);
				HelperUtility.closePort();
				if(!tmp) {
					showError("Could not write!");
					System.exit(1);
				}
			} else {
				showError("Could not open port!");
				System.exit(1);
			}
		}
	}

	private void showError(String error) {
		System.out.println(error);
		JOptionPane.showMessageDialog(this, error, "Error", JOptionPane.ERROR_MESSAGE);
	}

	private String askForSerialPort(String[] ports) {
		if ((ports != null) && (ports.length > 0)) {
			String s = (String)JOptionPane.showInputDialog(this, "Choose a serial port:",
				"Serial Port Selection", JOptionPane.PLAIN_MESSAGE, null,
				(Object[])ports, (Object)ports[0]);
			return s;
		} else {
			return null;
		}
	}

	public static void main (String[] args) {
		if (args.length == 0) {
			Remote r = new Remote();
			r.serialPort = r.askForSerialPort(HelperUtility.getPorts());
		} else {
			Remote r = new Remote(args[0]);
		}
	}
}

class PaintCanvas extends JPanel {

	private int[][] data = null;
	private int scale;

	public PaintCanvas(int w, int h, int magnify) {
		super();
		scale = magnify - 1;
		data = new int[(w / (scale + 1))][(h / (scale + 1))];
	}

	public void randomize() {
		java.util.Random r = new java.util.Random();
		int w = getWidth();
		int h = getHeight();
		for (int i = 0; i < (w / (scale + 1)); i++) {
			for (int j = 0; j < (h / (scale + 1)); j++) {
				data[i][j] = r.nextInt(256);
			}
		}
		repaint();
	}

	public void repaint() {
		repaint(0, 0, 0, getWidth(), getHeight());
	}

	public boolean isOpaque() {
		return true;
	}

	protected void paintComponent(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		for (int i = 0; i < (w / (scale + 1)); i++) {
			for (int j = 0; j < (h / (scale + 1)); j++) {
				g.setColor(new Color(data[i][j], data[i][j], data[i][j]));
				g.fillRect((i * (scale + 1)), (j * (scale + 1)), (i * (scale + 1)) + scale, (j * (scale + 1)) + scale);
			}
		}
	}
}