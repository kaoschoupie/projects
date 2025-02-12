
import java.time.LocalDateTime;

public class StockLog {

    private String quote;
    private String symbol;
    private LocalDateTime time;

    public StockLog(String quote, String symbol, LocalDateTime time) {
        this.quote = quote;
        this.symbol = symbol;
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

    public String getFormattedTime() {
        String rawTime = time.toString();
        String[] parts = rawTime.split("T");
        String hour = parts[1].substring(0, parts[1].indexOf('.'));
        return hour + " on date " + parts[0];
    }

}
