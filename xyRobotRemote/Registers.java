/*
 * Registers.java
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

class Registers extends JDialog implements ActionListener, ChangeListener, ItemListener {

	private Remote r = null;

	private JPanel stuff = null;
	private JLabel NName = null;
	private JCheckBox N = null;
	private JComboBox VH = null;
	private String VHs[] = { "No edge mode", "Horizontal edge", "Vertical edge", "2D edge" };
	private JLabel EName = null;
	private JLabel EVal = null;
	private JSlider E = null;
	private JLabel EMode1 = null;
	private JLabel EMode2 = null;
	private JCheckBox Extraction = null;
	private JCheckBox Enhancement = null;
	private JComboBox Z = null;
	private String Zs[] = { "No Calibration", "Positive", "Negative" };
	private JLabel IName = null;
	private JCheckBox I = null;
	private JSlider C = null;
	private JLabel CName = null;
	private JLabel CVal = null;
	private JSlider O = null;
	private JLabel OName = null;
	private JLabel OVal = null;
	private JSlider G = null;
	private JLabel GName = null;
	private JLabel GVal = null;
	// Allowed gain register values, and their possible counterparts...
	private int GAllowed[] = { 140, 155, 170, 185, 200, 215, 230, 245, 260, 275, 290, 305, 320, 350, 380, 410, 440, 455, 470, 515, 575 };
	private int GAllRegs[] = { 0,   1,   2,   3,   4,   5,   6,   7,   8,   21,  9,   23,  10,  11,  12,  13,  28,  14,  29,  15,  31  };
	private int GAltRegs[] = { 0,   1,   2,   3,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  14,  29,  30,  31  };
	private JSlider V = null;
	private JLabel VName = null;
	private JLabel VVal = null;

	private int P = 0, X = 0, M = 0;

	private JPanel regView = null;
	private JLabel regTitle = null;
	private JLabel regNames[] = { null, null, null, null, null, null, null, null };
	private JTextField regVals[] = { null, null, null, null, null, null, null, null };

	private JPanel centerPanel = null;
	private JButton convertToRaw = null;
	private JButton convertToNice = null;
	private JLabel tutorial = null;
	private JButton default1 = null;
	private JButton default2 = null;
	private JButton default3 = null;

	private final int default1Data[] = { 0x00, 0x03, 0x00, 0x01, 0x01, 0x00, 0x01, 0x04 };
	private final int default2Data[] = { 0x7F, 0x02, 0x00, 0x5A, 0x01, 0x00, 0x01, 0x04 };
	private final int default3Data[] = { 0x80, 0xD6, 0x06, 0x00, 0x01, 0x00, 0x01, 0x07 };

	public static void main(String[] args) {
		Registers registers = new Registers(null);
		registers.setVisible(true);
	}

	public Registers(Remote f, int[] regs) {
		this(f);
		convertVals(regs);
		setRegFields(regs);
	}

	public Registers(Remote f) {
		super(f, true);
		r = f;
		setTitle("Camera Register Configuration");
		setSize(410, 450);
		setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				if (r != null)
					r.registersUpdated(getRegisters());
				dispose();
			}
		});
		setLocationRelativeTo(f);

		Container c = getContentPane();
		setLayout(new BoxLayout(c, BoxLayout.LINE_AXIS));

		stuff = new JPanel();
		stuff.setPreferredSize(new Dimension(180, 400));
		stuff.setBorder(BorderFactory.createLoweredBevelBorder());
		stuff.setLayout(new FlowLayout());
		c.add(stuff);

		centerPanel = new JPanel();
		centerPanel.setLayout(new GridLayout(0, 1, 5, 5));
		centerPanel.setMaximumSize(new Dimension(70, 170));
		stuff.setBorder(BorderFactory.createLoweredBevelBorder());
		c.add(centerPanel);

		regView = new JPanel();
		regView.setLayout(new GridLayout(0, 2, 5, 5));
		regView.setMaximumSize(new Dimension(170, 300));
		regView.setBorder(BorderFactory.createLoweredBevelBorder());
		c.add(regView);

		for (int i = 0; i < 8; i++) {
			regNames[i] = new JLabel();
			if ((i < 4) || (i > 6)) {
				regNames[i].setText("Register " + i + ":");
			} else {
				if (i == 4) {
					regNames[i].setText("Register P:");
				} else if (i == 5) {
					regNames[i].setText("Register M:");
				} else {
					regNames[i].setText("Register X:");
				}
			}
			regView.add(regNames[i]);
			regVals[i] = new JTextField();
			regVals[i].setText("0x00");
			regView.add(regVals[i]);
		}

		tutorial = new JLabel();
		tutorial.setBorder(BorderFactory.createLoweredBevelBorder());
		tutorial.setText("<html>After closing this window, the values on the right"
					+ " are what is going into the Camera Registers. If you have used"
					+ " the GUI tools to change the configuration, press the "
					+ "Arrow-To-The-Right Button to convert the config.");
		// c.add(tutorial, BorderLayout.PAGE_END);

		convertToRaw = new JButton();
		convertToRaw.setText("-->");
		convertToRaw.addActionListener(this);
		centerPanel.add(convertToRaw);

		convertToNice = new JButton();
		convertToNice.setText("<--");
		convertToNice.addActionListener(this);
		centerPanel.add(convertToNice);
		
		default1 = new JButton();
		default1.setText("Def 1");
		default1.addActionListener(this);
		centerPanel.add(default1);

		default2 = new JButton();
		default2.setText("Def 2");
		default2.addActionListener(this);
		centerPanel.add(default2);

		default3 = new JButton();
		default3.setText("Def 3");
		default3.addActionListener(this);
		centerPanel.add(default3);

		NName = new JLabel();
		NName.setText("Exclusive Edge Mode:");
		stuff.add(NName);

		N = new JCheckBox();
		stuff.add(N);

		VH = new JComboBox(VHs);
		stuff.add(VH);

		EName = new JLabel();
		EName.setText("Edge Enh. Ratio:");
		stuff.add(EName);

		EVal = new JLabel();
		EVal.setText("200%");
		stuff.add(EVal);

		E = new JSlider(50, 500);
		E.addChangeListener(this);
		E.setValue(50);
		stuff.add(E);

		EMode1 = new JLabel();
		EMode1.setText("Ext.:");
		stuff.add(EMode1);

		Extraction = new JCheckBox();
		Extraction.addItemListener(this);
		Extraction.setSelected(false);
		stuff.add(Extraction);

		EMode2 = new JLabel();
		EMode2.setText("Enh.:");
		stuff.add(EMode2);

		Enhancement = new JCheckBox();
		Enhancement.addItemListener(this);
		Enhancement.setSelected(true);
		stuff.add(Enhancement);

		Z = new JComboBox(Zs);
		stuff.add(Z);

		IName = new JLabel();
		IName.setText("Invert output:");
		stuff.add(IName);

		I = new JCheckBox();
		stuff.add(I);

		CName = new JLabel();
		CName.setText("Exposure:");
		stuff.add(CName);

		CVal = new JLabel();
		CVal.setText("524ms");
		stuff.add(CVal);

		C = new JSlider(0, 65536);
		C.addChangeListener(this);
		C.setValue(524);
		stuff.add(C);

		OName = new JLabel();
		OName.setText("Offset Voltage:");
		stuff.add(OName);

		OVal = new JLabel();
		OVal.setText("0V");
		stuff.add(OVal);

		O = new JSlider(-31, 31);
		O.addChangeListener(this);
		O.setValue(0);
		stuff.add(O);

		GName = new JLabel();
		GName.setText("Gain:");
		stuff.add(GName);

		GVal = new JLabel();
		GVal.setText("22dB");
		stuff.add(GVal);

		G = new JSlider(140, 575);
		G.addChangeListener(this);
		G.setValue(140);
		stuff.add(G);

		VName = new JLabel();
		VName.setText("Output Voltage:");
		stuff.add(VName);

		VVal = new JLabel();
		VVal.setText("3.5V");
		stuff.add(VVal);

		V = new JSlider(0, 35);
		V.addChangeListener(this);
		V.setValue(35);
		stuff.add(V);

		convertSettingsToVals();
	}

	private int getRegVal(int i) {
		String val = regVals[i].getText();
		return Integer.valueOf(val.replaceAll("0x", ""), 16).intValue();
	}

	public int[] getRegisters() {
		int regs[] = { getRegVal(0), getRegVal(1), getRegVal(2), getRegVal(3), 
						getRegVal(4), getRegVal(5), getRegVal(6), getRegVal(7) };
		return regs;
	}

	private void convertValsToSettings() {
		convertVals(getRegisters());
	}

	private void convertVals(int regs[]) {
		// 0 (Z1, Z0, O5...O0)
		Z.setSelectedIndex(calcZ(regs[0]));
		O.setValue(calcO(regs[0]));

		// 1 (N, VH1, VH0, G4...G0)
		if ((regs[1] & 128) != 0) {
			N.setSelected(true);
		} else {
			N.setSelected(false);
		}
		calcVH(regs[1]);
		G.setValue(calcG(regs[1]));

		// 2 (C1) & 3 (C0)
		C.setValue(calcExposure(regs[2], regs[3]));

		// 4 & 5 & 6
		P = regs[4];
		M = regs[5];
		X = regs[6];

		// 7 (E3, E2, E1, E0, I, V2, V1, V0)
		E.setValue(calcE((regs[7] & 112) >> 4));
		if ((regs[7] & 128) != 0) {
			Extraction.setSelected(true);
			Enhancement.setSelected(false);
		} else {
			Extraction.setSelected(false);
			Enhancement.setSelected(true);
		}
		if ((regs[7] & 8) != 0) {
			I.setSelected(true);
		} else {
			I.setSelected(false);
		}
		V.setValue(calcV(regs[7] & 7));
	}

	private void setRegFields(int[] regs) {
		for (int i = 0; i < 8; i++) {
			String tmp = Integer.toHexString(regs[i]);
			if (tmp.length() == 1) {
				tmp = 0 + tmp;
			}
			regVals[i].setText("0x" + tmp.toUpperCase());
		}
	}

	private void convertSettingsToVals() {
		setRegFields(convertSettings());
	}

	private int[] convertSettings() {
		int regs[] = {0, 0, 0, 0, 0, 0, 0, 0};

		// 0 (Z1, Z0, O5...O0)
		calcZ(regs);
		calcO(regs);

		// 1 (N, VH1, VH0, G4...G0)
		if (N.isSelected()) { regs[1] |= (1 << 7); } // Set N bit, if desired
		calcVH(regs);
		calcG(regs);

		// 2 (C1) & 3 (C0)
		calcExposure(regs, C.getValue());

		// 4, P
		regs[4] = P;

		// 5, M
		regs[5] = M;

		// 6, X
		regs[6] = X;

		// 7 (E3, E2, E1, E0, I, V2, V1, V0)
		calcE(regs);
		if (I.isSelected()) { regs[7] |= (1 << 3); } // Set invert bit, if desired
		calcV(regs);

		return regs;
	}

	private int calcZ(int val) {
		return (val & 192) >> 6;
	}

	private void calcZ(int[] regs) {
		regs[0] &= ~(192); // Clear Z bits
		for (int i = 0; i < 3; i++) {
			if (Zs[i].equals(Z.getSelectedItem())) {
				regs[0] |= ((i & 3) << 6); // Move Z bits into place
				return;
			}
		}
	}

	private int calcO(int val) {
		int num = (val & 31);
		if ((val & 32) == 0) {
			num *= -1;
		}
		return num;
	}

	private void calcO(int[] regs) {
		regs[0] &= ~(63); // Clear O bits
		int o = O.getValue(); // From -31 to 31
		if (o > 0) {
			regs[0] |= 32; // Set O5
		} else {
			o *= -1;
		}
		regs[0] |= (o & 31); // Add o bits
	}

	private void calcVH(int val) {
		int i = (val & 96) >> 5;
		VH.setSelectedIndex(i);
	}

	private void calcVH(int[] regs) {
		regs[1] &= ~(96); // Clear VH bits
		for (int i = 0; i < 4; i++) {
			if (VHs[i].equals(VH.getSelectedItem())) {
				regs[1] |= ((i & 3) << 5); // Move VH bits into place
				return;
			}
		}
	}

	private int calcG(int val) {
		for (int i = 0; i < GAllowed.length; i++) {
			// Walk through all allowed values of g in GAllowed[i]
			if ((GAllRegs[i] == (val & 31)) || (GAltRegs[i] == (val & 31))) {
				// Found value
				return GAllowed[i];
			}
		}
		return GAllowed[0]; // Not found?
	}

	private void calcG(int[] regs) {
		int g = G.getValue();
		int found = 0;
		
		for (int i = 0; i < GAllowed.length; i++) {
			if (g == GAllowed[i]) {
				found = i;
			}
		}
		regs[1] &= ~(31); // Clear G bits
		regs[1] |= (GAllRegs[found] & 31); // Set bits
	}

	private int calcV(int flag) {
		return flag * 5;
	}

	private void calcV(int[] regs) {
		int flags = 0;
		int max = V.getValue();
		while (max >= 5) {
			flags++;
			max -= 5;
		}
		regs[7] &= ~(7); // Clear V bits
		regs[7] |= (flags & 7);
	}

	private int calcE(int flag) {
		switch (flag) {
			case 0:
				return 50;
			case 1:
				return 75;
			case 2:
				return 100;
			case 3:
				return 125;
			case 4:
				return 200;
			case 5:
				return 300;
			case 6:
				return 400;
			case 7:
				return 500;
			default:
				return 50;
		}
	}

	private void calcE(int[] regs) {
		int flags = 0;
		switch (E.getValue()) {
			case 75:
				flags = 1;
				break;
			case 100:
				flags = 2;
				break;
			case 125:
				flags = 3;
				break;
			case 200:
				flags = 4;
				break;
			case 300:
				flags = 5;
				break;
			case 400:
				flags = 6;
				break;
			case 500:
				flags = 7;
				break;
			default:
				flags = 0;
				break;
		}
		regs[7] &= ~(240); // Clear E bits
		if (Extraction.isSelected()) {
			regs[7] |= (1 << 7); // Set E3 --> Extraction mode
		}
		regs[7] |= (flags << 4); // Move flag into place.
	}

	private int calcExposure(int two, int three) {
		int val = three + (two << 8);
		return val;
	}

	private void calcExposure(int[] regs, int exposure) {
		int steps = exposure;
		regs[3] = (steps & 0x00FF);
		regs[2] = (steps & 0xFF00) >> 8;
	}

	public void itemStateChanged(ItemEvent e) {
		if (e.getSource().equals(Extraction)) {
			Enhancement.setSelected(!Extraction.isSelected());
		} else if (e.getSource().equals(Enhancement)) {
			Extraction.setSelected(!Enhancement.isSelected());
		}
	}

	public void actionPerformed(ActionEvent e) {
		if (e.getSource().equals(default1)) {
			setRegFields(default1Data);
			convertValsToSettings();
		} else if (e.getSource().equals(default2)) {
			setRegFields(default2Data);
			convertValsToSettings();
		} else if (e.getSource().equals(default3)) {
			setRegFields(default3Data);
			convertValsToSettings();
		} else if (e.getSource().equals(convertToNice)) {
			convertValsToSettings();
		} else if (e.getSource().equals(convertToRaw)) {
			convertSettingsToVals();
		}
	}

	public void stateChanged(ChangeEvent ev) {
		// For the sliders
		if (ev.getSource().equals(E)) {
			// Move slider to next valid pos...
			if (E.getValue() <= 62) {
				E.setValue(50);
			} else if (E.getValue() <= 87) {
				E.setValue(75);
			} else if (E.getValue() <= 112) {
				E.setValue(100);
			} else if (E.getValue() <= 162) {
				E.setValue(125);
			} else if (E.getValue() < 250) {
				E.setValue(200);
			} else if (E.getValue() < 350) {
				E.setValue(300);
			} else if (E.getValue() < 450) {
				E.setValue(400);
			} else {
				E.setValue(500);
			}
			EVal.setText(E.getValue() + "%");
		} else if (ev.getSource().equals(C)) {
			CVal.setText((C.getValue() * 16 / 1000) + "ms");
		} else if (ev.getSource().equals(O)) {
			float tmp = ((float)O.getValue()) / 31f;
			tmp = (float)(((int)(tmp * 10)) / 10f);
			OVal.setText(tmp + "V");
		} else if (ev.getSource().equals(G)) {
			int g = G.getValue();
			int found = 0;
			for (int i: GAllowed) {
				if (i <= g) {
					found = i;
				} else {
					break;
				}
			}

			G.setValue(found);
			float tmp = (float)found / 10f;
			GVal.setText(tmp + "dB");
		} else if (ev.getSource().equals(V)) {
			int v = V.getValue();
			if ((v % 5) != 0) {
				if ((v % 5) >= 3) {
					while ((v % 5) != 0) {
						v++;
					}
				} else {
					while ((v % 5) != 0) {
						v--;
					}
				}
			}
			V.setValue(v);
			float tmp = (float)v / 10f;
			VVal.setText(tmp + "V");
		}
	}
}
