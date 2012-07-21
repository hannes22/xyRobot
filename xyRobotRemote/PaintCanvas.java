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
import java.awt.image.*;

class PaintCanvas extends JPanel {

	public int[][] data = null;
	private int scale;
	private int w;
	private int h;

	private PaintCanvas toRefresh = null;

	public PaintCanvas(int width, int height, int magnify) {
		super();
		scale = magnify;
		w = width;
		h = height;
		data = new int[w][h];
	}

	public void setToRefresh(PaintCanvas p) {
		toRefresh = p;
	}

	public void testPatternA() {
		for (int x = 0; x < 128; x++) {
			for (int y = 0; y < 128; y++) {
				data[x][y] = y * 2;
			}
		}
		repaint();
	}

	public void testPatternB() {
		for (int x = 0; x < 128; x++) {
			for (int y = 0; y < 128; y++) {
				data[x][y] = x * 2;
			}
		}
		repaint();
	}

	public void testPatternC() {
		for (int x = 0; x < 128; x++) {
			for (int y = 0; y < 128; y++) {
				data[x][y] = x + y;
			}
		}
		repaint();
	}

	public void setData(short[] dat) {
		switch (dat.length) {
			case (128 * 128): default:
				setData(dat, 8);
				break;
			case ((128 * 128) / 2):
				setData(dat, 4);
				break;
			case ((128 * 128) / 4):
				setData(dat, 2);
				break;
			case ((128 * 128) / 8):
				setData(dat, 1);
				break;
		}
	}

	private void setData(int dat, int pos) {
		int i = 0;
		while (pos >= 128) {
			i++;
			pos -= 128;
		}
		data[i][pos] = dat;
	}

	public void setData(short[] dat, int depth) {
		int maxI = (128 * 128) / depthMax(depth);
		int maxJ = depthMax(depth);
		for (int i = 0; i < maxI; i++) {
			for (int j = 0; j < maxJ; j++) {
				setData(((dat[i] & (1 << (7 - j))) << j), (i * maxJ) + j);
			}
		}
		repaint();
	}

	private int depthMax(int depth) {
		switch (depth) {
			case 8: default:
				return 1;
			case 4:
				return 2;
			case 2:
				return 4;
			case 1:
				return 8;
		}
	}

	public void setDataSmall(short[] dat) {
		setData(dat, 4);
	}

	public void setData(int[][] dat) {
		for (int i = 0; i < dat.length; i++) {
			for (int j = 0; j < dat[i].length; j++) {
				data[i][j] = dat[i][j];
			}
		}
		repaint();
	}

	public void printData() {
		int lineCount = 0;
		for (int i = 0; i < 128; i++) {
			lineCount = 0;
			for (int j = 0; j < 128; j++) {
				if (lineCount++ < 40) {
					System.out.print(data[i][j] + " ");
				} else {
					System.out.println(data[i][j]);
					lineCount = 0;
				}
			}
		}
	}

	public void randomize(boolean rand) {
		java.util.Random r = new java.util.Random();
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				if (rand) {
					data[i][j] = r.nextInt(256);
				} else {
					data[i][j] = 0;
				}
			}
		}
		repaint();
	}

	public void repaint() {
		if (toRefresh != null) {
			toRefresh.setData(data);
		}
		repaint(0, 0, 0, getWidth(), getHeight());
	}

	public boolean isOpaque() {
		return true;
	}

	public RenderedImage paintIntoImage() {
		BufferedImage buff = new BufferedImage(w * scale, h * scale, BufferedImage.TYPE_INT_RGB);
		Graphics2D g = buff.createGraphics();
		paintComponent(g);
		g.dispose();
		return buff;
	}

	protected void paintComponent(Graphics g) {
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				if (data[i][j] > 255) {
					data[i][j] = 255;
				}
				if (data[i][j] < 0) {
					data[i][j] = 0;
				}
				g.setColor(new Color(data[i][j], data[i][j], data[i][j]));
				g.fillRect((i * scale), (j * scale), scale, scale);
			}
		}
	}
}