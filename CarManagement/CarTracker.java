/**
 * The CarTracke class is the user's tool to modify the cars data system. For example, the user can 
 * view the minimum price or mileage of a car in the car data structure. As well as, it can delete
 * or update cars in the cars data structure. 
 * 
 * @author  Lwin Oo
 * @version 1.0
 * @since   03/19/2020
 */

import java.io.IOException;
import java.util.Scanner;

public class CarTracker {
    private static Scanner scan;
    private static CarsManagingSystem car_manager;

    public static void main(String[] args) throws IOException {
        
        car_manager = new CarsManagingSystem(500,"cars2.txt");

        int userInput;
       
        do{
            //Display on therminal based
            userInput = display_menu();

            //choices
            switch(userInput){
                case 1: System.out.println("Adding the new car");
                        String vin, make, model, color;
                        int price, mileage;

                        scan = new Scanner(System.in);
                        System.out.print("\nEnter VIN:"); vin = scan.nextLine();
                        System.out.print("\nEnter Make:"); make = scan.nextLine();
                        System.out.print("\nEnter Model:"); model = scan.nextLine();
                        System.out.print("\nEnter Price:"); price = scan.nextInt();
                        System.out.print("\nEnter Mileage:"); mileage = scan.nextInt();
                        System.out.print("\nEnter Color:"); color = scan.next();
                        car_manager.insert(vin, make, model, price, mileage, color);
                        System.out.println("\nThere are " + car_manager.get_num_cars()+" cars in the system.");
                        scan.close();
                        
                break;

                case 2: System.out.println("Enter VIN:" + userInput);
                break;

                case 3: System.out.println("user entered:" + userInput);
                break;

                case 4: System.out.println("The Lowest Price cars");
                        car_manager.display_min_price_car();
                break;

                case 5: System.out.println("The Lowest Mileage cars");
                        car_manager.display_min_mileage_car();
                break;

                case 6: System.out.println("user entered: " + userInput);
                break;

                case 7: System.out.println("user entered: " + userInput);
                break;

                case 8: System.out.println("\n*** Good Bye!! ***\n");
                break;

                default: System.out.printf("\n\n***The choice \'%d\' is invalid! Please try again!***\n", userInput);
            }

        }while(userInput != 8);
        
    }

    /**
     * Display the user's menu on the terminal base
     */
    private static int display_menu(){

        scan = new Scanner(System.in);

        System.out.print("\n*****************************************************************\n"
            + "*\t\tWelcome to Car Searching Tool\t\t\t*\n" +
            "*****************************************************************\n" +
            "*  1: Add a Car\t\t\t\t\t\t\t*\n" +
            "*  2: Update a Car\t\t\t\t\t\t*\n" +
            "*  3: Remove a Car\t\t\t\t\t\t*\n" +
            "*  4: Retrieve the Lowest Price Car \t\t\t\t*\n" +
            "*  5: Retrieve the Lowest Mileage Car\t\t\t\t*\n" +
            "*  6: Retrieve the Lowest Price Car by Model and Make\t\t*\n"+
            "*  7: Retrieve the Lowest Mileage Car bye Model and Make\t*\n" +
            "*  8: Quit\t\t\t\t\t\t\t*\n" +
            "*****************************************************************\n\nEnter Choice: ");
            return scan.nextInt();
    }
}