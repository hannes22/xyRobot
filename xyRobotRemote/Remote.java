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
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import javax.imageio.*;
import java.io.*;

public class Remote extends JFrame implements KeyListener, ActionListener, ChangeListener {

	private final String version = "0.42";
	private final int width = 500;
	private final int height = 500;

	private PaintCanvas canvas = null;

	private JPanel serialStuff = null;
	private JComboBox portSelector = null;
	private JButton openPort = null;
	private JButton closePort = null;

	private JTextArea status = null;
	private JScrollPane statusScroll = null;

	private JPanel cameraStuff = null;
	private JButton trigger = null;
	private JButton save = null;
	private JSlider camMoveX = null;
	private JSlider camMoveY = null;
	private JButton camSettings = null;

	public SerialCommunicator serial = null;

	private final int UP    = 0;
	private final int DOWN  = 1;
	private final int LEFT  = 2;
	private final int RIGHT = 3;

	private int registers[] = {
		0x7F, // Z & Offset
		0x02, // N & VH & Gain
		0x00, // C1
		0x5A, // C0
		0x01, // P
		0x00, // M
		0x01, // X
		0x04  // E & I & V
	};

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
		canvas.setBorder(BorderFactory.createLoweredBevelBorder());

		status = new JTextArea("Initializing xyRobotRemote...");
		status.setBounds(10, 275, 256, 150);
		status.addKeyListener(this);
		status.setEditable(false);
		statusScroll = new JScrollPane(status);
		statusScroll.setBorder(BorderFactory.createLoweredBevelBorder());
		statusScroll.setBounds(10, 275, 256, 150);
		statusScroll.addKeyListener(this);
		c.add(statusScroll);

		serialStuff = new JPanel();
		serialStuff.setBorder(BorderFactory.createTitledBorder("Serial"));
		serialStuff.setBounds(275, 5, 215, 95);
		serialStuff.setLayout(null);
		serialStuff.addKeyListener(this);
		c.add(serialStuff);

		String[] ports = HelperUtility.getPorts();
		if ((ports == null) || (ports.length == 0)) {
			showError("No serial ports found!");
			System.exit(1);
		}
		java.util.List<String> list = java.util.Arrays.asList(ports);
		java.util.Collections.reverse(list);
		ports = (String[])list.toArray();
		for (int i = 0; i < ports.length; i++) {
			if (ports[i].equals("/dev/tty.xyRobot-DevB") && (i != 0)) {
				String tmp = ports[0];
				ports[0] = ports[i];
				ports[i] = tmp;
			}
		}
		portSelector = new JComboBox(ports);
		portSelector.addKeyListener(this);
		portSelector.setBounds(5, 20, 205, 30);
		serialStuff.add(portSelector);

		openPort = new JButton();
		openPort.setText("Open");
		openPort.setBounds(5, 55, 100, 30);
		openPort.addKeyListener(this);
		openPort.addActionListener(this);
		serialStuff.add(openPort);

		closePort = new JButton();
		closePort.setText("Close");
		closePort.setEnabled(false);
		closePort.setBounds(105, 55, 100, 30);
		closePort.addKeyListener(this);
		closePort.addActionListener(this);
		serialStuff.add(closePort);

		cameraStuff = new JPanel();
		cameraStuff.setBorder(BorderFactory.createTitledBorder("Camera"));
		cameraStuff.setBounds(275, 105, 215, 225);
		cameraStuff.setLayout(null);
		cameraStuff.addKeyListener(this);
		c.add(cameraStuff);

		trigger = new JButton();
		trigger.setText("Shoot Pic");
		trigger.setEnabled(false);
		trigger.setBounds(60, 15, 140, 30);
		trigger.addKeyListener(this);
		trigger.addActionListener(this);
		cameraStuff.add(trigger);

		camSettings = new JButton();
		camSettings.setText("Cam Registers");
		camSettings.setBounds(60, 50, 140, 30);
		camSettings.setEnabled(false);
		camSettings.addKeyListener(this);
		camSettings.addActionListener(this);
		cameraStuff.add(camSettings);

		save = new JButton();
		save.setText("Save image");
		save.setBounds(60, 85, 140, 30);
		save.addKeyListener(this);
		save.addActionListener(this);
		save.setEnabled(false);
		cameraStuff.add(save);

		camMoveY = new JSlider(JSlider.VERTICAL, 0, 180, 90); // vertical cam movement slider
		camMoveY.setEnabled(false);
		camMoveY.setBounds(5, 15, 60, 205);
		camMoveY.addKeyListener(this);
		camMoveY.addChangeListener(this);
		camMoveY.setMajorTickSpacing(45);
		camMoveY.setMinorTickSpacing(45);
		java.util.Hashtable<Integer, JLabel> labelTable = new java.util.Hashtable<Integer, JLabel>();
		labelTable.put(new Integer(0), new JLabel("Down"));
		labelTable.put(new Integer(180), new JLabel("Up"));
		camMoveY.setLabelTable(labelTable);
		camMoveY.setPaintTicks(true);
		camMoveY.setPaintLabels(true);
		cameraStuff.add(camMoveY);

		camMoveX = new JSlider(0, 180);
		camMoveX.setEnabled(false);
		camMoveX.setBounds(40, 120, 170, 60);
		camMoveX.addKeyListener(this);
		camMoveX.addChangeListener(this);
		camMoveX.setMajorTickSpacing(45);
		camMoveX.setMinorTickSpacing(45);
		labelTable = new java.util.Hashtable<Integer, JLabel>();
		labelTable.put(new Integer(0), new JLabel("Left"));
		labelTable.put(new Integer(180), new JLabel("Right"));
		camMoveX.setLabelTable(labelTable);
		camMoveX.setPaintTicks(true);
		camMoveX.setPaintLabels(true);
		cameraStuff.add(camMoveX);

		setVisible(true);

		serial = new SerialCommunicator(this);

		// Shutdown Hook to close an opened serial port
		Runtime.getRuntime().addShutdownHook(new Thread(new ShutdownThread(this), "Serial Closer"));

		log("Initialized!");
	}

	public void registersUpdated(int[] regs) {
		registers = regs;
	}

	public void setControls(boolean open) {
		closePort.setEnabled(open);
		openPort.setEnabled(!open);
		trigger.setEnabled(open);
		save.setEnabled(open);
		camMoveX.setEnabled(open);
		camMoveY.setEnabled(open);
		camSettings.setEnabled(open);
	}

	public void actionPerformed(ActionEvent e) {
		if (e.getSource().equals(camSettings)) {
			Registers reg = new Registers(this, registers);
			reg.setVisible(true);
		} else if (e.getSource().equals(trigger)) {
			serial.writeChar('c');
			log("Writing registers.");
			for (int i = 0; i < 8; i++) {
				serial.writeChar(registers[i]);
			}
			log("Getting picture data");
			canvas.setData(serial.readData(128 * 128));
			log("Done!");
		} else if (e.getSource().equals(openPort)) {
			// Open Port, enable controls
			if (serial.openPort((String)portSelector.getSelectedItem())) {
				if (serial.writeChar('?')) {
					String ver = serial.readLine();
					if (ver != null) {
						setControls(true);
						log("Connected to: " + ver);
					} else {
						// We are too fast, try again...
						if (serial.writeChar('?')) {
							ver = serial.readLine();
							if (ver != null) {
								setControls(true);
								log("Connected to: \"" + ver + "\"");
							} else {
								showError("No answer after second attempt!");
							}
						}
					}
				} else {
					showError("Could not send ping!");
				}
			} else {
				showError("Could not open port " + (String)portSelector.getSelectedItem());
			}
		} else if (e.getSource().equals(closePort)) {
			if (serial.isOpen())
				serial.closePort();
			setControls(false);
			log("Connection closed.");
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
					showError("Could not write!" + ex.getMessage());
				}
			}
		}
	}

	public void stateChanged(ChangeEvent e) {
		// For the sliders
		if (e.getSource().equals(camMoveY)) {
			if (!((JSlider)e.getSource()).getValueIsAdjusting()) {
				// New Y-Axis position
				serial.writeChar(0x80);
				serial.writeChar(camMoveY.getValue());
			}
		} else if (e.getSource().equals(camMoveX)) {
			if (!((JSlider)e.getSource()).getValueIsAdjusting()) {
				// New X-Axis position
				serial.writeChar(0x81);
				serial.writeChar(camMoveX.getValue());
			}
		}
	}

	public void keyReleased(KeyEvent e) {
		switch (e.getKeyChar()) {
			case 'r':
				canvas.randomize(true);
				break;

			case 'p':
				canvas.printData();
				break;

			case 'q':
				System.exit(0);
				break;
		}
	}

	public void showError(String error) {
		System.out.println(error);
		JOptionPane.showMessageDialog(this, error, "Error", JOptionPane.ERROR_MESSAGE);
		log("ERROR: " + error);
	}

	public void showInfo(String info) {
		JOptionPane.showMessageDialog(this, info, "Info", JOptionPane.INFORMATION_MESSAGE);
		log(info);
	}

	public void log(String log) {
		// System.out.println(log);
		status.append("\n" + log);
		status.setCaretPosition(status.getDocument().getLength());
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