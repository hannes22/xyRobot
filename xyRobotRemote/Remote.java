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
import javax.imageio.*;
import java.io.*;

public class Remote extends JFrame implements KeyListener, ActionListener {

	private final String version = "0.2";
	private final int width = 500;
	private final int height = 300;

	private PaintCanvas canvas = null;
	private JLabel title = null;
	private JComboBox portSelector = null;
	private JButton openPort = null;
	private JButton closePort = null;

	private JPanel cameraStuff = null;
	private JButton trigger = null;
	private JButton save = null;
	private JSlider exposure = null;

	public SerialCommunicator serial = null;

	public Remote() {
		super("xyRobotRemote");
		setSize(width, height);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		addKeyListener(this);
		setLayout(null);
		setResizable(false);
		Container c = getContentPane();

		canvas = new PaintCanvas(128, 128, 2);
		canvas.setBounds(10, 10, 256, 256);
		c.add(canvas);
		canvas.addKeyListener(this);

		title = new JLabel();
		title.setText("xyRobotRemote V " + version);
		title.setBounds(285, 10, 210, 20);
		title.addKeyListener(this);
		c.add(title);

		String[] ports = HelperUtility.getPorts();
		if ((ports == null) || (ports.length == 0)) {
			showError("No serial ports found!");
			System.exit(1);
		}
		java.util.List<String> list = java.util.Arrays.asList(ports);
		java.util.Collections.reverse(list);
		ports = (String[])list.toArray();
		portSelector = new JComboBox(ports);
		portSelector.addKeyListener(this);
		portSelector.setBounds(280, 35, 210, 30);
		c.add(portSelector);

		openPort = new JButton();
		openPort.setText("Open");
		openPort.setBounds(280, 70, 100, 30);
		openPort.addKeyListener(this);
		openPort.addActionListener(this);
		c.add(openPort);

		closePort = new JButton();
		closePort.setText("Close");
		closePort.setEnabled(false);
		closePort.setBounds(385, 70, 100, 30);
		closePort.addKeyListener(this);
		closePort.addActionListener(this);
		c.add(closePort);

		cameraStuff = new JPanel();
		cameraStuff.setBorder(BorderFactory.createTitledBorder("Camera"));
		cameraStuff.setBounds(275, 105, 215, 100);
		cameraStuff.setLayout(null);
		cameraStuff.addKeyListener(this);
		c.add(cameraStuff);

		trigger = new JButton();
		trigger.setText("Shoot Pic");
		trigger.setEnabled(false);
		trigger.setBounds(5, 15, 100, 30);
		trigger.addKeyListener(this);
		trigger.addActionListener(this);
		cameraStuff.add(trigger);

		save = new JButton();
		save.setText("Save image");
		save.setBounds(110, 15, 100, 30);
		save.addKeyListener(this);
		save.addActionListener(this);
		save.setEnabled(false);
		cameraStuff.add(save);

		exposure = new JSlider(0, 1048); // Exposure time in ms
		exposure.setMajorTickSpacing(262);
		exposure.setMinorTickSpacing(131);
		exposure.setEnabled(false);
		exposure.setBounds(5, 50, 205, 40);
		exposure.createStandardLabels(262);
		exposure.setPaintTicks(true);
		exposure.setPaintLabels(true);
		exposure.addKeyListener(this);
		cameraStuff.add(exposure);

		setVisible(true);

		serial = new SerialCommunicator(this);

		// Shutdown Hook to close an opened serial port
		Runtime.getRuntime().addShutdownHook(new Thread(new ShutdownThread(this), "Serial Closer"));
	}

	public void setControls(boolean open) {
		closePort.setEnabled(open);
		openPort.setEnabled(!open);
		trigger.setEnabled(open);
		save.setEnabled(open);
		exposure.setEnabled(open);
	}

	public void actionPerformed(ActionEvent e) {
		if (e.getSource().equals(openPort)) {
			// Open Port, enable controls
			if (serial.openPort((String)portSelector.getSelectedItem())) {
				if (serial.writeChar('?')) {
					String ver = serial.readLine();
					if (ver != null) {
						setControls(true);
						showInfo("Connected to " + ver);
					}
				}
			}
		} else if (e.getSource().equals(closePort)) {
			if (serial.isOpen())
				serial.closePort();
			setControls(false);
		} else if (e.getSource().equals(save)) {
			// Render canvas into image
			JFileChooser chooser = new JFileChooser();
			if (chooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
				String path = chooser.getSelectedFile().getAbsolutePath();
				if (!path.endsWith(".png")) {
					path += ".png";
				}
				File f = new File(path);
				try {
					ImageIO.write(canvas.paintIntoImage(), "png", f);
				} catch (Exception ex) {
					showError("Could not write!\n" + ex.getMessage());
				}
			}
		}
	}

	public void keyReleased(KeyEvent e) {
		switch (e.getKeyChar()) {
			case 'r':
				canvas.randomize(true);
				break;

			case 'q':
				System.exit(0);
				break;
		}
	}

	public void showError(String error) {
		System.out.println(error);
		JOptionPane.showMessageDialog(this, error, "Error", JOptionPane.ERROR_MESSAGE);
	}

	public void showInfo(String info) {
		JOptionPane.showMessageDialog(this, info, "Info", JOptionPane.INFORMATION_MESSAGE);
	}

	public static void main (String[] args) {
		Remote r = new Remote();
	}

	public void keyPressed(KeyEvent e) { }
	public void keyTyped(KeyEvent e) { }
}

class ShutdownThread implements Runnable {
	private Remote remote = null;

	public ShutdownThread(Remote r) {
		remote = r;
	}

	public void run() {
		if (remote.serial != null) {
			if (remote.serial.isOpen()) {
				remote.serial.closePort();
			}
		}
	}
}