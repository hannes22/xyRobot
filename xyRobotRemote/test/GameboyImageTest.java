/*
 * By: Thomas Buck <taucher.bodensee@gmail.com>
 * Visit: www.xythobuz.org
 */

import java.awt.*;
import java.io.*;
import java.awt.image.*;
import javax.imageio.*;

public class GameboyImageTest {

	BufferedImage image = null;
	int[] dat = null;

	GameboyImageTest(String img) {
		try {
			image = ImageIO.read(getClass().getResourceAsStream(img));
			
			// Resize
			BufferedImage newImage = new BufferedImage(128, 128, BufferedImage.TYPE_INT_RGB);
			Graphics2D g = newImage.createGraphics();
			g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
			g.drawImage(image, 0, 0, 128, 128, null);
			g.dispose();
			image = newImage;

			Raster r = image.getData();
			int[] data = new int[128 * 128];
			for (int x = 0; x < 128; x++) {
				for (int y = 0; y < 128; y++) {
					int[] pixelData = r.getPixel(x, y, (int[])null);
					int average = 0;
					for (int i = 0; i < pixelData.length; i++) {
						average += pixelData[i];
					}
					average /= pixelData.length;
					// data[x + (128 * y)] = average;
					data[y + (128 * x)] = average;
				}
			}
			dat = data;
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
			System.exit(1);
		}
	}

	void writeData(String file) {
		try {
			FileWriter fw = new FileWriter(file);
			BufferedWriter bw = new BufferedWriter(fw);
			for (int i = 0; i < dat.length; i++) {
				bw.write("" + dat[i]);
				if (i < (dat.length - 1)) {
					bw.write(",");
					if (((i + 1) % 32) == 0) {
						bw.write("\n");
					}
				}
			}
			bw.close();
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
			System.exit(1);
		}
	}

	public static void main (String[] args) {
		if (args.length != 3) {
			System.out.println("Usage: GameboyImageTest image.png out.txt");
			System.exit(1);
		}

		GameboyImageTest gb = new GameboyImageTest(args[0]);
		gb.writeData(args[1]);
	}
}
