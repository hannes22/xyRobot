/*
 * DistanceWindow.java
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

class DistanceWindow extends JFrame {

	private Remote r = null;
	private JLabel[] l = null;
	private int max = 0;

	public final int width = 150;
	public int height;

	private final int lights = 20;

	public DistanceWindow(Remote parent) {
		this(parent, 80);
	}

	public DistanceWindow(Remote parent, int maxVal) {
		super();
		setResizable(false);
		setLayout(null);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				r.distanceWindowKilled();
				dispose();
			}
		});
		r = parent;
		max = maxVal;

		height = r.height;
		setBounds(r.width + r.xOff, r.yOff, width, height);
		
		l = new JLabel[lights];
		for (int i = 0; i < l.length; i++) {
			l[l.length - 1 - i] = new JLabel("" + (4 + (max * (l.length - 1 - i) / l.length)));
			l[l.length - 1 - i].setHorizontalAlignment(SwingConstants.CENTER);
			l[l.length - 1 - i].setOpaque(true);
			l[l.length - 1 - i].setBackground(Color.GRAY);
			l[l.length - 1 - i].setBounds(5, 5 + (i * ((height - 34) / lights)), (width - 10), (((height - 34) / lights) - 5));
			add(l[l.length - 1 - i]);
		}
	}

	public void setVal(int val) {
		int step = max / lights;
		int height = val / step;
		for (int i = 0; i < lights; i++) {
			if (i < height) {
				if (i < (lights / 4)) {
					l[i].setBackground(Color.RED);
				} else if (i < (2 * lights / 4)) {
					l[i].setBackground(Color.YELLOW);
				} else {
					l[i].setBackground(Color.GREEN);
				}
			} else {
				l[i].setBackground(Color.GRAY);
			}
		}
	}
}
