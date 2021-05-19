import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;
import java.util.regex.Pattern;

/**
 * This class manages the car data structure. Also, it load the user's given
 * file into the data structure. Then, it collect the result from the minimum
 * priority queue data structures. It delievers the result to the CarTracker
 * class.
 * 
 * @author Lwin Oo
 * @version 1.0
 * @since 03/19/2020
 * 
 */

 public class CarsManagingSystem{
    private final char price = 'p';
    private final char mile = 'm';
     
    /**
     * Attributes of minimum priority queues and this class
     */
    private CarsMinPQ min_price;
    private CarsMinPQ min_mileage;

    private int number_of_cars;
    private Scanner fscan;
    private File file;

    /**
     * Constructor for the CarsManagingSystem object
     * 
     */
    public CarsManagingSystem(int capacity, String fname) throws IOException{

         if(capacity <= 0) throw new IllegalArgumentException("The size should not less than or equal to zero.");

         min_price = new CarsMinPQ(capacity, price);
         min_mileage = new CarsMinPQ(capacity, mile);
         this.number_of_cars = 0;
         pre_insert_cars(fname);
    }

    private void pre_insert_cars(String fname) throws IOException{

      try{
         String vin, make, model, color;
         int price, mileage;
         
         //file accessing and scanning
         file = new File(fname);
         fscan = new Scanner(file);

         if(fscan.equals(null)) throw new FileNotFoundException("The given file do not exist.");
         
         //building up the cars data structure 
         while(fscan.hasNextLine()){
             vin = fscan.useDelimiter(Pattern.compile(":")).next();
             if(vin.length() <= 16){
                 fscan.nextLine();
                 continue;
             }
             make = fscan.useDelimiter(Pattern.compile(":")).next();
             model = fscan.useDelimiter(Pattern.compile(":")).next();
             price = fscan.useDelimiter(Pattern.compile(":")).nextInt();
             mileage = fscan.useDelimiter(Pattern.compile(":")).nextInt();
             fscan.skip(":");
             color = fscan.nextLine();
             add_car(vin, make, model, price, mileage, color);        
         }
         fscan.close();
     }catch(IOException e){
         throw new IOException(e);
     }
    }

    /**
     * the numbers of cars
     * @return
     */
    public int get_num_cars(){ return this.number_of_cars+1;}

    /**
     * 
     * @return
     */
    public CarsObject get_min_price_car(){return min_price.get_min();}

    /**
     * 
     * @return
     */
    public CarsObject get_min_mileage_car(){return min_mileage.get_min();}

    /**
     * Display the minimum price car
     */
    public void display_min_price_car(){
      System.out.println(min_price.get_min().toString());
    }

    /**
     * Display the minimum mileage car
     */
    public void display_min_mileage_car(){
      System.out.println(min_mileage.get_min().toString());
    }


    /**
     * 
     * @param vin
     * @param make
     * @param model
     * @param price
     * @param mileage
     * @param color
     */
    private void add_car(String vin, String make, String model, int price, int mileage, String color) {
       CarsObject new_car = new CarsObject(vin, make, model, price, mileage, color);
       this.number_of_cars++;
       min_price.insert(get_num_cars(), new_car);
       min_mileage.insert(get_num_cars(), new_car);       
    }

	public void insert(String vin, String make, String model, int price2, int mileage, String color) {
	}
    

    
 }