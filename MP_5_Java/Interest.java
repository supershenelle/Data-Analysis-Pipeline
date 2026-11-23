/********************
Last names: Danieles, De Guzman, Nono, Suazon
Language: Java
Paradigm(s): Object-Oriented, Procedural
********************/

import java.util.*;

/**
 * This class contains the computation of daily interest, annual interest, and expected daily interest.
 * Each daily entry is represented by a record which contains the day, interest, and updated balance.
 * List of entries are created for easy access, especially when called during the menu class.
 */

public class Interest {

    private static final double ANNUAL_RATE = 0.05;
    // Represents a day's interest entry
    public record DailyEntry(int day, double interest, double balance){}

    /**
     * Rounds the amount to 2 decimal places.
     * @param amount is the amount to be rounded off
     * @return the rounded of monetary amount to 2 decimal places
     */

    private double roundAmt(double amount){
        return Math.round(amount * 100.0) / 100.0;
    }

    /**
     * Computes the interest earned in a single day on the given balance.
     * @param balance represents the balance to compute interest on.
     * @return the daily interest amount as a double.
     */
    public double dailyInterest(double balance){
        return balance * (ANNUAL_RATE / 365);
    }

    /**
     * Computes the annual interest on the given balance.
     * @param balance represents the balance to compute interest on.
     * @return the annual interest amount as a double.
     */
    public double annualInterest(double balance){
        return roundAmt(balance * ANNUAL_RATE);
    }

    /**
     * Simulates the daily compound interest for the given number of days.
     * @param days represents the number of days to simulate.
     * @param startBalance represents the starting balance.
     * @return a list of DailyEntry records, one record per day.
     */
    public List<DailyEntry> expectedDailyInterest(int days, double startBalance){
        ArrayList<DailyEntry> entries = new ArrayList<>(); // creates the arrayList that stores each day entry / record 
        
        double base = roundAmt(startBalance);
        double daily = roundAmt(base * (ANNUAL_RATE/ 365));
        double runningBalance = base;

        for(int i=0; i < days; i++){
            runningBalance = roundAmt(runningBalance + daily);
            entries.add(new DailyEntry(i+1, daily, runningBalance));
        }

        return entries;
    } 
}