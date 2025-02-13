
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.ObjectOutputStream;
import java.net.HttpURLConnection;
import java.time.LocalDateTime;

public class StockLog {

    private String quote;
    private String symbol;
    private LocalDateTime time;

    public StockLog(String quote, String symbol, LocalDateTime time) {
        this.quote = quote;
        this.symbol = symbol.toUpperCase();
        this.time = time;
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

        System.out.println("The value of the stock " + stock.getSymbol().toUpperCase() + " is " + result + "$ at " + stock.getFormattedTime());

        ObjectOutputStream write = null;
        try {
            write = new ObjectOutputStream(new FileOutputStream("log.bin", true));
            write.writeObject(stock);
            write.close();
        } catch (Exception e) {
            System.out.println("Failed to write the log");
        }

    }

    @Override
    public String toString() {
        String output = "The stock " + symbol + " trading at " + quote + "$ at " + getFormattedTime();
        return output;
    }

}
