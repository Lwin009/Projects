/********************************************************************************************************
 * @author  Lwin Oo                                                                                     *
 * @date    2/13/2020                                                                                   *
 * @version 1.0                                                                                         *
 * @about   CS1501 class project number II. In this project, the user can compress a file. User will    *
 *          use the terminal emulator and command to compress a file. The user must use the following   *
 *          command as its described...                                                                 *
 *          User command:                                                                               *
 *                                                                                                      *
 *              :   java MyLZW - n < input_file > new_file.lzw  (compressing do nothing mode)           *
 *              :   java MyLZW - r < input_file > new_file.lzw  (compressing reset mode)                *
 *              :   java MyLZW - m < input_file > new_file.lzw  (compressing monitor mode)              *
 *                                                                                                      *
 *              : java MyLZW + < input_file.lzw > original_file (decompressing)                         *
 ********************************************************************************************************/

public class MyLZW {
    private static final int R = 256;                               // number of input chars
    private static final int MAX_W = 16;                            // codeword width
    private static final int MAX_L = (int) Math.pow(2, MAX_W);      // number of codewords = 2^MAX_W
    private static int W = 9;                                       //initial codeword width
    private static int L = (int) Math.pow(2, W);                   //current number of codewords = 2
    private static String mode = "";            //Mode such as n, r, m
    private static final double THRESHOLD = 1.1;
    private static double currentCompressRatio = 0;
    private static double oldCompressRatio = 0;
    private static double newCompressRatio = 0;
    private static int bitCompressed = 0;
    private static int bitUncompressed = 0;

    /********************************************************************
     * Compress a file based on the user's preference mode. There are   *
     * three type of modes such as 'n' Do nothing mode, 'r' Reset mode, *
     * and 'm' Monitor mode.                                            *
     * i. Do nothing mode: the file will be compress without creating   *
     * the new code book when the code book is full. This algorithm is  *
     * same as the original LZW algorithm.                              *
     * ii. Reset mode: the file will be compress respectively, and the  *
     * code book will be reset (create/add new code book) when the code *
     * book is full.                                                    *
     * iii. Monitor mode: the file will be compress based on the        *
     * the compression ration. If the compression ration is less than   *
     * 1.1 keep compression with do nothing mode, otherwise use the     *
     * reset mode.                                                      *
     ********************************************************************/
    public static void compress() {

        //firstly write the mode at the beginning of the file
        if (mode.equals("r") || mode.equals("n") || mode.equals("m"))
            BinaryStdOut.write(mode, 8);

        String  input = BinaryStdIn.readString();   //read in all the file
        TST<Integer> st = new TST<Integer>();
        for (int i = 0; i < R; i++)
            st.put("" + (char) i, i);
        int code = R+1;  // R is codeword for EOF

        switch (mode){
            case "n":   //'n' mode
                while (input.length() > 0) {
                    //Code width Increasing
                    if (code == Math.pow(2, W) && W != MAX_W){
                        W++;
                        // System.out.print(" "+W +":::");       //debug 1.1
                    }
                    String s = st.longestPrefixOf(input);  // Find max prefix match s.
                    BinaryStdOut.write(st.get(s), W);      // Print s's encoding.
                    //System.out.print(":"+st.get(s) + "  ");   //debug 1.2
                    int t = s.length();
                    if (t < input.length() && code < Math.pow(2, W)) {    // Add s to symbol table.
                        st.put(input.substring(0, t + 1), code++);
                    }
                    input = input.substring(t);            // Scan past s in input.
                }

                break;

            case "r":       //'r' mode

                while (input.length() > 0) {

                    //code book width increasing
                    if (code == Math.pow(2, W) && W < MAX_W){
                        W++;
                        //st.put(input.substring(0, t + 1), code++);
                    }

                    String s = st.longestPrefixOf(input);  // Find max prefix match s.
                    BinaryStdOut.write(st.get(s), W);      // Print s's encoding.
                    //System.out.print(":"+st.get(s) + "  ");   //debug 1.2
                    int t = s.length();
                    if (t < input.length() && code < Math.pow(2, W)) {    // Add s to symbol table.
                        st.put(input.substring(0, t + 1), code++);
                    }
                    input = input.substring(t);            // Scan past s in input.

                    //resting the Code-book
                    if (code == Math.pow(2, MAX_W)) {

                        st = new TST<Integer>();
                        for (int i =0; i < R; i++)
                            st.put("" + (char) i, i);
                        code = R+1;
                        W = 9;
                    }
                }

                break;

            case "m":       //'m' mode

                while (input.length() > 0) {

                    String s = st.longestPrefixOf(input);  // Find max prefix match s.
                    int write = st.get(s);
                    BinaryStdOut.write(write, W);      // Print s's encoding.

                    bitUncompressed += (s.length() * 8);
                    bitCompressed += W;
                    newCompressRatio = (double) bitUncompressed / bitCompressed;

                    //System.out.print(":"+st.get(s) + "  ");   //debug 1.2
                    int t = s.length();
                    if (t < input.length() && code < Math.pow(2, W)) {    // Add s to symbol table.
                        st.put(input.substring(0, t + 1), code++);
                    }

                    //code book width increasing
                    if (code == Math.pow(2, W) && W < MAX_W){
                        W++;
                        st.put(input.substring(0, t + 1), code++);
                    }

                    //if code-book is full check the ratio of the compression
                    if (code == Math.pow(2, W)){
                        if (oldCompressRatio == 0){
                            oldCompressRatio = newCompressRatio;
                        }

                        currentCompressRatio = oldCompressRatio/ newCompressRatio;

                        if (currentCompressRatio > THRESHOLD){

                            st = new TST<Integer>();
                            for (int i =0; i < R; i++)
                                st.put("" + (char) i, i);
                            code = R+1;
                            W = 9;

                            oldCompressRatio = 0;
                            newCompressRatio = 0;

                        }

                    }

                    input = input.substring(t);            // Scan past s in input.

                }

                break;
            default:

        }

        BinaryStdOut.write(R, W);
        BinaryStdOut.close();
    }

    /********************************************************************
     * Decompress a file based on 8 bits at the beginning of the        *
     *.file. If the 8 bits is 'n' that is Do nothing mode, 'r' that is  *
     * Reset mode, and 'm' that is Monitor mode.                        *
     * i. Do nothing mode: the file will be decompress without creating *
     * the new code book when the code book is full. This algorithm is  *
     * same as the original LZW algorithm, except that W increase till  *
     * MAX_W, 16 bits.
     * ii. Reset mode: the file will be decompress respectively, and the*
     * code book will be reset (create/add new code book) when the code *
     * book is full.                                                    *
     * iii. Monitor mode: the file will be decompress based on the      *
     * the compression ration. If the compression ration is less than   *
     * 1.1 keep compressing with the do nothing mode, otherwise use the *
     * reset mode.                                                      *
     ********************************************************************/
    public static void expand() {
        String[] st = new String[MAX_L];

        int i; // next available codeword value
        mode = Character.toString(BinaryStdIn.readInt(8));

        // initialize symbol table with all 1-character strings
        for (i = 0; i < R; i++)
            st[i] = "" + (char) i;
        st[i++] = "";                        // (unused) lookahead for EOF

        int codeword = BinaryStdIn.readInt(W);
        if (codeword == R) return;           // expanded message is empty string
        String val = st[codeword];

        switch(mode){
            case "n":

                while (true) {
                    //Code width Increasing
                    if ( (i+1) == Math.pow(2, W) && W != MAX_W) {
                        W++;
                    }

                    //System.out.print(":"+codeword +"  ");      //debug 2.1
                    BinaryStdOut.write(val);
                    codeword = BinaryStdIn.readInt(W);
                    if (codeword == R) break;
                    String s = st[codeword];
                    if (i == codeword) s = val + val.charAt(0);   // special case hack
                    if (i < MAX_L)
                        st[i++] = val + s.charAt(0);
                    val = s;

                }
                break;
            case "r":

                while (true) {

                    //Code width Increasing
                    if (i+1 == Math.pow(2, W) && W != MAX_W) {
                        W++;
                        //st[i++] = val + s.charAt(0);
                    }

                    BinaryStdOut.write(val);
                    codeword = BinaryStdIn.readInt(W);
                    if (codeword == R) break;
                    String s = st[codeword];
                    if (i == codeword) s = val + val.charAt(0);   // special case hack
                    if (i < MAX_L)
                        st[i++] = val + s.charAt(0);

                    //resting the Code-book
                    if ((i+1) >= Math.pow(2, MAX_W)) {

                        st = new String[MAX_L];
                        // initialize symbol table with all 1-character strings
                        for (i = 0; i < R; i++)
                            st[i] = "" + (char) i;
                        st[i++] = "";                        // (unused) lookahead for EOF
                        W = 9;

                        codeword = BinaryStdIn.readInt(W);
                        if (codeword == R) return;           // expanded message is empty string
                        val = st[codeword];
                    }

                    val = s;

                }
                break;
            case "m":

                while (true) {

                    BinaryStdOut.write(val);
                    codeword = BinaryStdIn.readInt(W);

                    if (codeword == R) break;
                    String s = st[codeword];
                    if (i == codeword) s = val + val.charAt(0);   // special case hack
                    if (i < Math.pow(2, W))
                        st[i++] = val + s.charAt(0);

                    //Code width Increasing
                    if (i+1 == Math.pow(2, W) && W != MAX_W) {
                        W++;
                        st[i++] = val + s.charAt(0);
                    }

                    val = s;

                    bitUncompressed += val.length()*8;
                    bitCompressed +=W;
                    newCompressRatio = (double) bitUncompressed/bitCompressed;

                    //resting the Code-book
                    if (i+1 == Math.pow(2, MAX_W)) {

                        if (oldCompressRatio == 0){
                            //bitUncompressed += val.length() * 8;
                          //  newCompressRatio = (double) bitUncompressed/bitCompressed;
                            oldCompressRatio = newCompressRatio;
                        }

                        currentCompressRatio = oldCompressRatio/newCompressRatio;
                        if (currentCompressRatio > THRESHOLD){
                            st = new String[MAX_L];
                            // initialize symbol table with all 1-character strings
                            for (i = 0; i < R; i++)
                                st[i] = "" + (char) i;
                            st[i++] = "";                        // (unused) lookahead for EOF
                            W = 9;

                            codeword = BinaryStdIn.readInt(W);
                            if (codeword == R) return;           // expanded message is empty string
                            val = st[codeword];

                            oldCompressRatio = 0;
                            newCompressRatio = 0;
                        }
                    }
                }

                break;
            default:
        }
        BinaryStdOut.close();
    }



    public static void main(String[] args) {

        if(args[0].equals("-") && args[1].equals("n")){

            mode = args[1];       //do nothing mode
            compress();                     //compress the file

        }else if(args[0].equals("-") && args[1].equals("r")){

            mode = args[1];       //reset mode
            compress();                     //compress the file

        }else if(args[0].equals("-") && args[1].equals("m")){

            mode = args[1];       //monitor mode
            compress();                     //compress the file

        }else if(args[0].equals("+")){

            expand();                       //expand the file
        }else{
            throw new IllegalArgumentException("Illegal command line argument");
        }
    }

}
