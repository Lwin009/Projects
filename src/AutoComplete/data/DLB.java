package AutoComplete.data;

/**
 * This is a DLB data structure which was taught in the Data Structure and Algorithm course 
 * from the University of Pittsburgh. This data structure can be used to store new data and 
 * search data that already existed in the structure. For this reason, this class only offers
 * two methods which are insert and search. 
 * 
 * @author      Lwin Oo
 * @date        01/02/2022
 * @version     1.0.0 
 */

public class DLB {
    
    // root Node where all the data start growing
    private Node root;

    // the end character for each word
    private final char END = '$';

    
    public DLB(){
        root = null;
    }

    /**
     * Insert a word into the DLB data structure
     * @param word to be inserted in the data structure
     * @return true if a word successfully inserted, false otherwise.
     */
    public boolean insert(String word){

        // Checking the word Integrity
        if(word == null || word.contains("!#$%^*")) 
            throw new IllegalArgumentException("Invalid Arguments");
        
        // Add the given word into the data structure
        if( (root = add(root, word + END, word, 0)) != null)
            return true;

        return false;
    }

    /**
     * Helper Method, it insert the given value (a word) into a 
     * @param node      a new node to assign with a new key
     * @param key       each character of a given word 
     * @param value     value store at the end of a word.
     * @param length    the given word's lenght or the number of characters
     */
    private Node add(Node node, String key, String value, int length){

        // temporary node or current node
        Node currNode = node;

        // base case for the recursion
        if(length == key.length()) return currNode; 

        // current character from the given key
        char c = key.charAt(length);

        // determind if the given node is null or contain
        if(currNode == null){

            if(c == END) currNode = new Node(c, value);
            else currNode = new Node(c, null);
            currNode.setChild(add(currNode.getChild(), key, value, length+1));

        }else {

            if(currNode.getKey() == c){

                // currNode = add(currNode.getChild(), key, value, length + 1);
                currNode.setChild(add(currNode.getChild(), key, value, length+1));
            }else {

                Node sibling = currNode;

                while(sibling.getSibling() != null){
                    if(sibling.getKey() == c){
                        sibling = sibling.getChild();
                        break;
                    }
                    sibling = sibling.getSibling();
                }

                if(sibling.getKey() == c || (sibling.getChild() == null && sibling.getKey() != END)){
                    sibling.setChild(add(sibling.getChild(), key, value, length+1));
                }else{
                    sibling.setSibling(add(sibling.getSibling(), key, value, length));
                }
            }

        }

        return currNode;
    }


    /**
     * Search the prefix in the DLB data structure.
     * 
     *** If the given string does not exist, it returns 0.
     *** If the given string is a word but not a prefix to another word, 
     *      then it return 1.
     *** If the given string is not a word and a prefix to another word, 
     *      then it return 2.
     *** If the given string is a word as well as a prefix, then it 
     *      returns 3;
     * 
     * @param str string to be searched
     * @return  0 if it is not a word
     *          1 if it is a word but not a prefix
     *          2 if it is not a word, but is a prefix
     *          3 if it is a word and a prefix
     */
    private int searchPrefix(String str){

        // data trie 
        Node currNode = this.root;

        // throw exception if the root is null or trie is empty
        if(currNode == null) throw new NullPointerException("The DLB Trie is empty.");

        for(int i = 0; i < str.length(); i++){

            if(currNode.getKey() == str.charAt(i)){
                
                currNode = currNode.getChild();
            }else{

                // get the sibling until find the correct character and sibling
                while(currNode.getSibling() != null){
                    
                    if(currNode.getKey() == str.charAt(i)){
                        break;
                    }

                    currNode = currNode.getSibling();
                }

                if(currNode.getKey() != str.charAt(i)) return 0;

                else i--;


            }

        }
        
        if(currNode.getKey() == END && currNode.getSibling() == null){
            return 1;
        }else if(currNode.getChild() != null && currNode.getKey() != END){
            return 2;
        }else if(currNode.getValue().equals(str) && currNode.getSibling() != null){
            return 3;
        }

        return 0;
    }

    


}
