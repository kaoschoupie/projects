
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.net.HttpURLConnection;
import java.time.LocalDateTime;
import java.util.ArrayList;

public class StockLog implements Serializable {

    public static final String FILENAME = "log.bin";
    private String quote;
    private String symbol;
    private LocalDateTime time;

    public StockLog(String quote, String symbol, LocalDateTime time) {
        this.quote = quote;
        this.symbol = symbol.toUpperCase();
        this.time = time;
    }

    public StockLog(StockLog other) {
        this(other.quote, other.symbol, other.time);
    }

    public StockLog() {
        this(null, null, LocalDateTime.now());
    }

    public void setQuote(String quote) {
        this.quote = quote;
    }

    public void setSymbol(String symbol) {
        this.symbol = symbol;
    }

    public void setTime(LocalDateTime time) {
        this.time = time;
    }

    public String getQuote() {
        return quote;
    }

    public String getSymbol() {
        return symbol;
    }

    public LocalDateTime getTime() {
        return time;
    }

    /**
     * Method to get the hour and the date in a neatly formatted way to avoid
     * having seconds and decimals
     *
     * @return String that is the formatted hour and date
     */
    public String getFormattedTime() {
        String rawTime = time.toString();
        String[] parts = rawTime.split("T");
        String hour = parts[1].substring(0, parts[1].indexOf('.'));
        return hour + " on date " + parts[0];
    }

    /**
     * Method to retrieve a certain stock's quote and log the created object
     * into an object file
     *
     * @param symbol String that is the stock symbol (Eg: aapl)
     * @param connection HttpURLConnection object
     */
    public static void retrieveStockQuote(String symbol, HttpURLConnection connection) {
        BufferedReader reader = null;
        String line = null;

        String token = "data-testid=\"qsp-price\">";
        try {
            reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));

            while ((line = reader.readLine()) != null) {

                //System.out.println(line);
                if (line.contains(token)) {
                    break;
                    //expect = false;
                }
            }

            reader.close();
        } catch (Exception e) {
            System.out.println("Couldn't read html page");
            System.exit(1);
        }

        int index = line.indexOf(token);

        String subLine = line.substring(index);

        String endToken = "</span>";

        int endIndex = subLine.indexOf(endToken);

        String result = subLine.substring(token.length(), endIndex - 1);

        LocalDateTime date = (java.time.LocalDateTime.now());

        StockLog stock = new StockLog(result, symbol, date);

        System.out.println("The value of the stock " + stock.getSymbol().toUpperCase() + " is " + result + "$ at "
                + stock.getFormattedTime());

        ObjectOutputStream write = null;
        ArrayList<StockLog> currentLog = readLog();
        if (currentLog == null) {
            currentLog = new ArrayList<>();
        }
        currentLog.add(stock);
        try {
            write = new ObjectOutputStream(new FileOutputStream(FILENAME));
            write.writeObject(currentLog);
            write.close();
        } catch (Exception e) {
            System.out.println("Failed to write the log");
        }

    }

    public static ArrayList<StockLog> readLog() {
        ObjectInputStream reader = null;
        try {
            reader = new ObjectInputStream(new FileInputStream(FILENAME));

            ArrayList<StockLog> log = (ArrayList<StockLog>) reader.readObject();

            reader.close();
            return log;

        } catch (Exception e) {
            return null;
        }
    }

    public static void clearLog() {
        ObjectOutputStream write = null;
        try {
            write = new ObjectOutputStream(new FileOutputStream(FILENAME));
            write.writeObject(null);
            write.close();
        } catch (Exception e) {

        }
    }

    @Override
    public String toString() {
        String output = "The stock " + symbol + " trading at " + quote + "$ at " + getFormattedTime();
        return output;
    }

}
