/*
 * CanvasWindow.java
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
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

class CanvasWindow extends JFrame {

	private Remote r = null;
	private PaintCanvas p = null;

	public CanvasWindow(Remote parent) {
		super("Camera Viewer");
		setResizable(false);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				r.cameraWindowKilled();
				dispose();
			}
		});
		r = parent;

		setBounds(0, 0, 512, 534);
		p = new PaintCanvas(128, 128, 4);
		add(p);

		setVisible(true);
	}

	public PaintCanvas getCanvas() {
		return p;
	}

	public void setCanvas(PaintCanvas pc) {
		p = pc;
	}
}