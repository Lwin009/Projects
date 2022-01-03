/******************************************************************************************************
 * This project is a completion of the school project when I was at the University of Pittsburgh.
 * In this project, text messager like windows would apper as the program start running. The user 
 * could type any characters or letters, then there would be suggestion words in a box with four 
 * different choice of related word to the user's entry. 
 * 
 * For example: If a user types the "app", then there would be four suggestion of "app", "appable",
 * "appanda", "appanet".
 * 
 * 
 * @author		Lwin Oo
 * @date 		6/30/2021
 * @version 	0.0.0.1
 * 
 *******************************************************************************************************/
 
 import javax.swing.JFrame;

 protected class AutoFrame extends JFrame 
 {
 	final int WINDOW_WIDTH = 250;		// the frame width
 	final int WINDOW_HEIGH = 450;		// the frame heigh

 	/**
 	 * Constructor
 	 */
 	public AutoFrame(){

 		// set the window name
 		setTitle("AutoComplete");

 		// set the window size
 		setSize(WINDOW_WIDTH, WINDOW_HEIGH);

 		setDefaultCloseOperation(JFrame.EXT_ON_CLOSE);

 		setVisible(true);

 	}
 }