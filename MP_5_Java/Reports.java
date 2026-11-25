import java.io.*;
import java.time.*;
import java.time.format.DateTimeFormatter;
import java.util.*;

// Load data once
// groups the data according to the account
// create the reports: loop

/**
 * This class represents the report class containing each function necessary to generate each individual task
 */
public class Reports{

    private LocalDate latestDate; // newest date sa file
    private double totalInterest = 0;
    private HashMap<Long, ArrayList<TransactionRecords.Transaction>> byAccount = new HashMap<>(); // hashmap that maps each account number to their list of transactions

    // records that represent a line in each respective report
    public record TierPerAcc(String name, long age, double balance, double avg, String tier){} // record that represents info of report1
    public record InterestPerUser(String name, double balance, double interest){} // record that represents report2
    public record ActivityPerUser(String name, LocalDate date, double totalWithdrawalAmounts, int numberOfDeposits, double totalDepositAmounts,
                                double turnoverVelocity, String riskFlagLevel){} // record that represents report3
    public record TopUser(String name, long accountNumber, int activityCount){} // record that represents report4


    /**
     * The constructor for the reports class
     * @param rows is an arraylist that represents each row of transactions found in the file
     */
    public Reports(ArrayList<TransactionRecords.Transaction> rows){

        // gets the date in the first row
        latestDate = rows.get(0).date();

        // goes thru every row to get latest date and fill the hashmap
        for(TransactionRecords.Transaction t: rows){

            /**
             * If element without no account num as that key exists yet create it with the list
             * If meron, simply add that transaction to the element's list
             */
            if(!byAccount.containsKey(t.accountNumber())){
                byAccount.put(t.accountNumber(), new ArrayList<>());
            }
            byAccount.get(t.accountNumber()).add(t);

            // This gets the latest date from all the transactions
            if(t.date().isAfter(latestDate)){
                latestDate = t.date();
            }
        }
    }


    // Helper files

    /**
     * This is a reusable function that returns the monetary amount in the specified format: divided by commas with 2 decimal places.
     * @param is a double that represents the amount to be formatted
     * @return a string of formatted amount
     */
    public String formatMoney(double amount){
        return "\"" + String.format("%,.2f", amount) + "\"";
    }

    /**
     * This is a reusable function that appends specific information onto the csv file and displays the preview.
     * @param fileName is a string that represents the name of the file to be created
     * @param header is a string that represents the header of the file
     * @param lines is an arraylist of strings that contains each line of rows to be written onto the file
     */
    public void reportsOnCsv(String fileName, String header, ArrayList<String> lines){
        try(PrintWriter out = new PrintWriter(new FileWriter(fileName, false))){
            out.println(header);

            for(String line: lines){
                out.println(line);
            }
        } catch(IOException e){
            System.out.println("Error: Could not save to " + fileName);
            return;
        }


        // for the table preview for every report
        printPreviewTable(header, lines);
        System.out.println();
        System.out.println("(Full table exported to " + fileName + ")");
        System.out.println();
    }

    /**
     * This function creates the preview table for each report generated
     * @param header is a string that represents the header of the report.
     * @param lines is a string that represents each row/line written on the report.
     */
    public void printPreviewTable(String header, ArrayList<String> lines){
        
        // gets header and first two rows
        ArrayList<String> content = new ArrayList<>();
        content.add(header);
        
        for(int i=0; i< Math.min(2, lines.size()); i++){
            content.add(lines.get(i));
        }

        // splits into each column
        ArrayList<String[]> table = new ArrayList<>();
        for(String line: content){
            String[] parts = line.split("\""); 
            String row = "";
            for(int j=0; j<parts.length; j++){
                if(j%2 == 0){
                    row +=parts[j].replace(",", ";");
                } else {
                    row += parts[j];
                }
            }
            table.add(row.split(";"));
        }

        // columns gets as wide as the longest value para equal
        int[] widths = new int[table.get(0).length];
        for(String[] cols: table){
            for(int k=0; k< cols.length; k++){
                widths[k] = Math.max(widths[k], cols[k].length());
            }
        }

        for(String[] cols: table){
            for(int c=0; c <cols.length; c++){
                System.out.printf("%-" + widths[c] + "s |  ", cols[c]);
            }
            System.out.println();
        }
    }

    
    /**
     * This function generates the report1: Banking Privilege Tiers.
     * It goes through every account and its transaction history computing the account's age, average end of day balance,
     * and account tier.
     */
    public void report1(){
        System.out.println("Report 1: Banking Privilege Tiers");
        ArrayList<TierPerAcc> tierRecords = new ArrayList<>(); // creates a list of tierRecords/ rows to in the file

        // data of latest date sa files
        int year = latestDate.getYear();
        int month = latestDate.getMonthValue();
        int daysInMonth = latestDate.lengthOfMonth();

        // Goes through every account and computes required info
        for(ArrayList<TransactionRecords.Transaction> list: byAccount.values()){
            // gets first and recent transaction of a user
            TransactionRecords.Transaction first = list.get(0);
            TransactionRecords.Transaction last = list.get(list.size()-1);
        

            // computes the average end of day balance
            // goes thru every day in the month
            double sum = 0;
            for(int i=1; i<= daysInMonth; i++){
                LocalDate date = LocalDate.of(year, month, i);
                double endOfDayBalance = 0;

                // computes current balance in that day
                for(TransactionRecords.Transaction t: list){
                    if(!t.date().isAfter(date)){
                        endOfDayBalance = t.currentBalance();
                    } else {
                        break;
                    }
                }
                sum = sum + endOfDayBalance; // adds after each day
            }
            double avg = sum / daysInMonth; // gets the average

            // To determine account tier
            if(avg >= 100000){
                long age = latestDate.toEpochDay() - first.date().toEpochDay();
                String tier;
                if(avg < 500000){
                    tier = "Silver";
                } else if (avg < 1000000){
                    tier = "Gold";
                } else{
                    tier = "Platinum";
                }

                tierRecords.add(new TierPerAcc(first.accountName(), age, last.currentBalance(), avg, tier));
            }
        }

        // Sort based on highest average
        tierRecords.sort((a,b) -> Double.compare(b.avg(), a.avg()));

        // create the list of records na iwrrite sa csv
        ArrayList<String> lines = new ArrayList<>();
        for(TierPerAcc r: tierRecords){
            lines.add(r.name() + "," + r.age() + "," + formatMoney(r.balance()) + "," + formatMoney(r.avg()) + "," + r.tier());
        }

        reportsOnCsv("report1_banking_privilege_tiers.csv",  "AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier", lines);
    }


    /**
     * This function iterates through every account and its transaction history
     * to compute for their expected interest alongside the the required information to be appended on the csv file.
     */
    public void report2(){
        System.out.println("Report 2: Interest Expense Projection");
        ArrayList<InterestPerUser> interestRecords = new ArrayList<>();
        Interest ints = new Interest();

        // gets current balance lang based on the user's last transaction
        // computes interest and add to interst records
        for(ArrayList<TransactionRecords.Transaction> list: byAccount.values()){
            TransactionRecords.Transaction last = list.get(list.size()-1);

            if(last.currentBalance() >= 10000){
                double interest = ints.dailyInterest(last.currentBalance()) * 30;
                totalInterest += interest;
                interestRecords.add(new InterestPerUser(last.accountName(), last.currentBalance(), interest));
            }
        }
        
        // sort in descending order
        interestRecords.sort((a,b) -> Double.compare(b.interest(), a.interest()));

        ArrayList<String> lines = new ArrayList<>();
        for(InterestPerUser r: interestRecords){
            lines.add(r.name() + "," +formatMoney( r.balance()) + "," + formatMoney(r.interest()));
        }

        reportsOnCsv("report2_interest_expense_projection.csv", "AccountName,CurrentBalance,ExpectedInterest", lines);
    }

    /**
     * This function iterates through every acount and its transaction history to determine any
     * suspicious client behavior
     */
    public void report3(){
        System.out.println("Report 3: Suspicious Client Behavior");
        ArrayList<ActivityPerUser> activityRecords = new ArrayList<>();

        // Iterates through every arraylist owned by an account
        for(ArrayList<TransactionRecords.Transaction> list: byAccount.values()){
            int i = 0;
            while(i < list.size()){
                LocalDate date = list.get(i).date(); // gets date of this list
                double totalWithdrawalAmounts = 0;
                double totalDepositAmounts= 0;
                int numDeposists = 0;

                // collecs every transaction on the same date
                while(i < list.size() && list.get(i).date().equals(date)){
                    if(list.get(i).type().equals("withdrawal")){
                        totalWithdrawalAmounts += list.get(i).amount();
                    } else{
                        numDeposists += 1;
                        totalDepositAmounts += list.get(i).amount();
                    }

                    i+=1;
                }

                double turnoverVelocity; 
                String flag;

                // computes turnoverVelocity
                if(totalDepositAmounts == 0){
                    turnoverVelocity = 0;
                } else{
                    turnoverVelocity = totalWithdrawalAmounts / totalDepositAmounts * 100;
                }

                // Flags if suspicious or normal
                if(totalWithdrawalAmounts >= 500000 || turnoverVelocity >= 90){
                    flag = "Suspicious";
                } else{
                    flag = "Normal";
                }

                activityRecords.add(new ActivityPerUser(list.get(0).accountName(), date, totalWithdrawalAmounts, numDeposists, totalDepositAmounts, turnoverVelocity, flag));
            }
        }

        // sorts based on date
        activityRecords.sort((a,b) -> {
            int byDate = a.date().compareTo(b.date());
            if(byDate != 0){
                return byDate;
            }

            return b.riskFlagLevel().compareTo(a.riskFlagLevel());
        });
        
        // creates string/line of rows to write
        ArrayList<String> lines = new ArrayList<>();
        DateTimeFormatter fmt = DateTimeFormatter.ofPattern("dd/MM/yyyy");
        for(ActivityPerUser r: activityRecords){
            lines.add(r.name() + "," + r.date().format(fmt) + "," + formatMoney(r.totalWithdrawalAmounts()) + "," + r.numberOfDeposits() + "," +
            formatMoney(r.totalDepositAmounts()) + "," + formatMoney(r.turnoverVelocity()) + "," + r.riskFlagLevel());
        }

        reportsOnCsv("report3_suspicious_client_behavior.csv", "AccountName,Date,TotalWithdrawalAmounts,NumberOfDeposits,TotalDepositAmounts,TurnoverVelocity,RiskFlagLevel", lines);
    
    }


    /**
     * Used to generate the top 10 users based on their activity in the past month
     */
    public void report4(){
        System.out.println("Report 4: Top 10 Most Active Users");
        ArrayList<TopUser> topUsers = new ArrayList<>();

        // gets year and month of current date
        int year = latestDate.getYear();
        int month = latestDate.getMonthValue();
        int activityCount;

        for(ArrayList<TransactionRecords.Transaction> list: byAccount.values()){
            activityCount = 0;
            for(TransactionRecords.Transaction t: list){
                if(t.date().getYear() == year && t.date().getMonthValue() == month){
                    activityCount+=1;
                }
            }

            // after checking transaction history store to the list
            topUsers.add(new TopUser(list.get(0).accountName(), list.get(0).accountNumber(), activityCount));
        }

        topUsers.sort((a,b) -> Integer.compare(b.activityCount(), a.activityCount()));
        ArrayList<String> lines = new ArrayList<>();

        for(int i = 0; i < Math.min(10, topUsers.size()); i ++){
            lines.add((i+1) + "," + topUsers.get(i).name() + "," + topUsers.get(i).accountNumber() + "," + topUsers.get(i).activityCount());
        }

        reportsOnCsv("report4_top_10_active_users.csv", "Rank,AccountName,AccountNumber,TransactionCount", lines);
    }
    
    public void summary(){
        double totalDeposits = 0;
        double totalWithdrawals = 0;
        double totalBalance = 0;

        // iterates through every account, then through every transaction history
        for(ArrayList<TransactionRecords.Transaction> row: byAccount.values()){
            for(TransactionRecords.Transaction t: row){
                if(t.type().equals("deposit")){
                    totalDeposits += t.amount();
                } else{
                    totalWithdrawals += t.amount();
                }
            }
            totalBalance += row.get(row.size()-1).currentBalance(); // adds the balance
        }

        String jsonOutput = String.format("{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}",
                byAccount.size(), totalDeposits, totalWithdrawals, totalBalance, totalInterest);
        String jsonFile = String.format("{\n" +
                            "  \"total_accounts\": %d,\n" +
                            "  \"total_deposits\": %.2f,\n" +
                            "  \"total_withdrawals\": %.2f,\n" +
                            "  \"total_balance\": %.2f,\n" +
                            "  \"total_interest_expense\": %.2f\n" +
                            "}",
                            byAccount.size(), totalDeposits, totalWithdrawals, totalBalance, totalInterest);

        try(PrintWriter out = new PrintWriter(new FileWriter("summary.json", false))){
            out.println(jsonFile);
        } catch(IOException e){
            System.out.println("Error: Could not save to summary.json");
            return;
        }

        System.out.println("Summary State (summary.json):");
        System.out.println(jsonOutput);
    }
}
