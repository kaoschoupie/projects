// Java Program to get connected
// To Web Server

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;
import java.time.LocalDateTime;

// Driver Class
public class WebServerConnection {

    public static HttpURLConnection establishConnection(String url) {
        URL serverUrl = null;
        try {
            serverUrl = new URI(url).toURL();
        } catch (Exception e) {

        }
        // Step 3: Open a connection
        HttpURLConnection connection = null;
        try {
            connection = (HttpURLConnection) serverUrl.openConnection();

            connection.setRequestProperty("User-Agent", "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.4; en-US; rv:1.9.2.2) Gecko/20100316 Firefox/3.6.2");

            // Step 4: Set the request method to GET
            connection.setRequestMethod("GET");

            // Step 5: Get the HTTP response code
            int responseCode = connection.getResponseCode();

            System.out.println("Response Code: " + responseCode);
        } catch (Exception e) {
            System.out.println(e);
            return null;
        }
        return connection;
    }

    // main function
    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("Please input a stock in the command");
            System.exit(0);
        }
        try {
            // Step 1: Define the URL
            // Replace with your desired URL
            String url = "https://ca.finance.yahoo.com/quote/" + args[0];

            HttpURLConnection connection = establishConnection(url);
            if (connection == null) {
                System.out.println("Failed to connect");
                System.exit(1);
            }

            // Step 2: Create a URL object
            // Step 6: Read and display response content
            BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));

            String line;

            String token = "data-testid=\"qsp-price\">";

            while ((line = reader.readLine()) != null) {

                //System.out.println(line);
                if (line.contains(token)) {
                    break;
                    //expect = false;
                }
            }

            int index = line.indexOf(token);

            String subLine = line.substring(index);

            String endToken = "</span>";

            int endIndex = subLine.indexOf(endToken);

            String result = subLine.substring(token.length(), endIndex - 1);

            LocalDateTime date = (java.time.LocalDateTime.now());
            String rawTime = date.toString();
            String[] parts = rawTime.split("T");
            String hour = parts[1].substring(0, parts[1].indexOf('.'));

            System.out.println("The value of the stock " + args[0].toUpperCase() + " is " + result + "$ at " + hour + " on date " + parts[0]);

            reader.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}
