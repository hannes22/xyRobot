/*
 * PaintCanvas.java
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

	public void setData(Object[] dat) {
		short[] d = new short[dat.length];
		for (int i = 0; i < d.length; i++) {
			d[i] = (short)Integer.valueOf((String)dat[i]).intValue();
		}
		setData(d);
	}

	public void setData(int[] dat) {
		short[] dat2 = new short[dat.length];
		for (int i = 0; i < dat.length; i++) {
			dat2[i] = (short)dat[i];
		}
		setData(dat2);
	}

	public void setData(short[] dat) {
		switch (dat.length) {
			case (128 * 128):
				setData(dat, 1);
				break;
			case ((128 * 128) / 2):
				setData(dat, 2);
				break;
			case ((128 * 128) / 4):
				setData(dat, 4);
				break;
			case ((128 * 128) / 8):
				setData(dat, 8);
				break;

			default:
				System.out.println("Data length not valid!");
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
		int maxI = (128 * 128) / depth;
		for (int i = 0; i < maxI; i++) {
			for (int j = 0; j < depth; j++) {
				setData(((dat[i] & (1 << (7 - j))) << j), (i * depth) + j);
			}
		}
		repaint();
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

	public void randomize() {
		java.util.Random r = new java.util.Random();
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				data[i][j] = r.nextInt(256);
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
