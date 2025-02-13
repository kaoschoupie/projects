
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;

public class WebServerConnection {

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

            connection.setRequestProperty("User-Agent", "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.4; en-US; rv:1.9.2.2) Gecko/20100316 Firefox/3.6.2");

            connection.setRequestMethod("GET");

            int responseCode = connection.getResponseCode();

            System.out.println("Response Code: " + responseCode);
        } catch (Exception e) {
            System.out.println(e);
            return null;
        }
        return connection;
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("Please input a stock in the command");
            System.exit(0);
        }

        String url = "https://ca.finance.yahoo.com/quote/" + args[0];

        HttpURLConnection connection = establishConnection(url);
        if (connection == null) {
            System.out.println("Failed to connect");
            System.exit(1);
        }

        StockLog.retrieveStockQuote(args[0], connection);

    }

}
