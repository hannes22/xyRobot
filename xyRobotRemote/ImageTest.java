/*
 * ImageTest.java
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
import java.awt.*;
import java.io.*;
import java.awt.image.*;
import javax.imageio.*;

public class ImageTest {

	private int[] data = null;

	ImageTest(String img) {
		boolean b = false;
		try {
			BufferedImage image = ImageIO.read(getClass().getResourceAsStream(img));
			
			// Resize
			BufferedImage newImage = new BufferedImage(128, 128, BufferedImage.TYPE_INT_RGB);
			Graphics2D g = newImage.createGraphics();
			g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
			g.drawImage(image, 0, 0, 128, 128, null);
			g.dispose();
			image = newImage;

			Raster r = image.getData();
			data = new int[128 * 128];
			for (int x = 0; x < 128; x++) {
				for (int y = 0; y < 128; y++) {
					int[] pixelData = r.getPixel(x, y, (int[])null);
					int average = 0;
					for (int i = 0; i < 3; i++) {
						average += pixelData[i];
					}
					average /= 3;

					data[y + (128 * x)] = average;
					if (data[y + (128 * x)] > 255) {
						data[y + (128 * x)] = 255;
					}
				}
			}
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
		}
	}

	int[] getData() {
		return data;
	}

	void writeData(BufferedWriter bw) {
		try {
			for (int i = 0; i < data.length; i++) {
				bw.write("" + data[i]);
				if (i < (data.length - 1)) {
					bw.write(",");
					if (((i + 1) % 32) == 0) {
						bw.write("\n");
					}
				}
			}
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
		}
	}

	void writeData(String file) {
		try {
			FileWriter fw = new FileWriter(file);
			BufferedWriter bw = new BufferedWriter(fw);
			writeData(bw);
			bw.close();
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
		}
	}

	public static void main (String[] args) {
		if (args.length != 3) {
			System.out.println("Usage: ImageTest image.png out.txt");
			System.exit(1);
		}

		ImageTest gb = new ImageTest(args[0]);
		gb.writeData(args[1]);
	}
}
