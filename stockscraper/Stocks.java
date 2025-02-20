
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;
import java.util.ArrayList;
import java.util.InputMismatchException;
import java.util.Scanner;

public class Stocks {

    final static String BASE_URL = "https://ca.finance.yahoo.com/quote/";

    /**
     * Method to establish connection to a website
     *
     * @param url string that is a url
     * @return valid HttpURLConnection object
     */
    public static HttpURLConnection establishConnection(String url) {
        URL serverUrl = null;
        try {
            serverUrl = new URI(url).toURL();
        } catch (Exception e) {

        }

        HttpURLConnection connection = null;
        try {
            connection = (HttpURLConnection) serverUrl.openConnection();

            connection.setRequestProperty("User-Agent",
                    "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.4; en-US; rv:1.9.2.2) Gecko/20100316 Firefox/3.6.2");

            connection.setRequestMethod("GET");

            int responseCode = connection.getResponseCode();

            if (responseCode > 200) {
                if (responseCode == 404) {
                    System.out.println("Accessed an inexistant stock, check symbol spelling");
                    return null;

                }
                System.out.println("Failed to connect");
                System.exit(0);
            }

            //System.out.println("Response Code: " + responseCode);
        } catch (Exception e) {
            System.out.println(e);
            return null;
        }
        return connection;
    }

    public static void menu() {
        System.out.println("Stocks information\n"
                + "Options:\n"
                + "1. Get a stock quote\n"
                + "2. Read quotes' log\n"
                + "3. Clear log\n"
                + "4. Get one stock's log\n"
                + "0. Exit\n"
        );
    }

    public static void main(String[] args) {
        Scanner kb = new Scanner(System.in);
        while (true) {
            menu();
            System.out.print("Enter your choice: ");
            int option = 0;

            try {
                option = kb.nextInt();
            } catch (InputMismatchException e) {
                System.out.println("\nPlease enter a number\n");
                continue;
            }

            switch (option) {
                case 1: {
                    System.out.print("Input a stock's symbol: ");
                    String symbol = kb.next();
                    String url = BASE_URL + symbol;

                    HttpURLConnection connection = establishConnection(url);
                    if (connection == null) {
                        System.out.println("Failed to connect, check spelling");
                        continue;
                    }

                    StockLog.retrieveStockQuote(symbol, connection);
                    break;
                }

                case 2: {
                    ArrayList<StockLog> log = StockLog.getLog();
                    for (int idx = 0; idx < log.size(); idx++) {
                        System.out.println(log.get(idx));

                    }
                    break;
                }

                case 3: {
                    System.out.println("Are you sure you want to clear the log? This action is irreversible");
                    System.out.print("Y/n:");
                    String choice = kb.next();
                    if (choice.equals("Y")) {
                        StockLog.clearLog();
                        System.out.println("Log cleared");
                    }

                    break;
                }

                case 4: {
                    System.out.println("Which stock history do you want to retrieve from the log (input the symbol):");
                    String symbol = kb.next().toUpperCase();
                    ArrayList<StockLog> stock = StockLog.getStockLog(symbol);
                    if (stock.isEmpty()) {
                        System.out.println("You have no recorded log for the stock " + symbol);
                        break;
                    }
                    for (int idx = 0; idx < stock.size(); idx++) {
                        System.out.println(stock.get(idx));
                    }
                    break;
                }

                case 5: {

                    break;
                }

                case 0: {
                    System.out.print("Goodbye");
                    kb.close();
                    System.exit(0);
                }
                default: {
                    System.out.println("Please enter a valid option");
                }

            }

        }
    }

}
