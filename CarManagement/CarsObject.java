/**
 * This class is Car object that it holds vin number, make, model, price, mileage, and the color values.
 * It also contain accessor and mutator methods to modify the object. 
 * @author  Lwin Oo
 * @version 1.0
 * @since   03/19/2020
 * 
 */
public class CarsObject {

    private String _VIN;
    private String _MAKE;
    private String _MODEL;
    private int _PRICE;
    private int _MILEAGE;
    private String _COLOR;

    /**
     * Constructor for creating an empty Object
     */
    public CarsObject(){
        this("", "", "", 0, 0, "");
    }

    /**
     * Constructor that assign the values in CarsObject Object
     * @param vin the vin number of the car
     * @param make the company made of the car
     * @param model the model of the car
     * @param price the price of the car
     * @param mileage the mileage of the car
     * @param color the color of the car
     */
    public CarsObject(String vin, String make, String model, int price, int mileage, String color){
        _VIN = vin;
        _MAKE = make;
        _MODEL = model;
        _PRICE = price;
        _MILEAGE = mileage;
        _COLOR = color;
    }

     /**
     * Set the VIN number of the car
     * @param vin
     */
    public void set_vin(String vin){
        _VIN = vin;
    }

    /**
     * Get the VIN number of the car
     * @return the VIN
     */
    public String get_vin(){
        return _VIN;
    }

    /**
     * Set the brand of the Car
     * @param make
     */
    public void set_make(String make){
        _MAKE = make;
    }

    /**
     * Return the brand of the car
     * @return the brand (make)
     */
    public String get_make(){
        return _MAKE;
    }

    /**
     * Set the car's model
     * @param model 
     */
    public void set_model(String model){
        _MODEL = model;
    }

    /**
     * Return the model of the car
     * @return the model
     */
    public String get_model(){
        return _MODEL;
    }

    /**
     * Set the price of the car
     * @param price 
     */
    public void set_price(int price){
        _PRICE = price;
    }

    /**
     * Return the price of the car
     * @return
     */
    public int get_price(){
        return _PRICE;
    }

    /**
     * Set the mileage of the car
     * @param mileage
     */
    public void set_mileage(int mileage){
        _MILEAGE = mileage;
    }

    /**
     * Return the mileage of the car
     * @return
     */
    public int get_mileage(){
        return _MILEAGE;
    }

    /**
     * Set the color of the car
     * @param color
     */
    public void set_color(String color){
        _COLOR = color;
    }

    /**
     * Return the color of the car
     * @return
     */
    public String get_color(){
        return _COLOR;
    }

    /**
     * Return the String to display
     */
    @Override
    public String toString(){
        return "Vin: " + _VIN +"\nMake: " + _MAKE +
                        "\nModel: "+ _MODEL + "\nPrice: " + _PRICE + "\nMileage: "+
                        _MILEAGE + "\nColor: " + _COLOR ;
    }

}
