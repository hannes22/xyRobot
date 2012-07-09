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
import java.awt.image.*;

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

	public RenderedImage paintIntoImage() {
		BufferedImage buff = new BufferedImage(data.length, data[0].length, BufferedImage.TYPE_INT_RGB);
		Graphics2D g = buff.createGraphics();
		paintComponent(g);
		g.dispose();
		return buff;
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