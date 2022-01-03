package AutoComplete.data;

/**
 * This is a helper class for the DLB Data Structure class. This class serves as a data
 * container or data holder.
 * 
 * @author      Lwin Oo
 * @date        01/02/2022
 * @version     1.0.0 
 */

public class Node {

    private Node sibling;
    private Node child;
    private char key;
    private String value;
    
    public Node(){
        sibling = null;
        child = null;
        key = '!';
        value = null;
    }

    public Node(char key, String value){
        this.sibling = null;
        this.child = null;
        this.key = key;
        this.value = value;
    }

    public Node(Node sibling, char key, String value, Node child){
        this.sibling = sibling;
        this.key = key;
        this.value = value;
        this.child = child;
    }

    /**
     * Set this Node's sibling to given sibling
     * @param sibling next sibling
     */
    public void setSibling(Node sibling){
        this.sibling = sibling;
    }

    /**
     * Set this Node's child to given child
     * @param child next child 
     */
    public void setChild(Node child){
        this.child = child;
    }

    /**
     * Set this Node's key to given a new key
     * @param key new key
     */
    public void setKey(char key){
        this.key = key;
    }

    /**
     * Set this Node's value to given a new value
     * @param value new value
     */
    public void setValue(String value){
        this.value = value;
    }

    /**
     * Return this Node's sibling 
     * @return the sibling
     */
    public Node getSibling(){
        return this.sibling;
    }

    /**
     * Return this Node's child
     * @return the child
     */
    public Node getChild(){ 
        return this.child;
    }

    /**
     * Return this Node's key
     * @return the key
     */
    public char getKey(){
        return this.key;
    }  

    /**
     * Return this Node's value
     * @return the value
     */
    public String getValue(){
        return this.value;
    }


}
