import pandas as pd
import bs4 as BeautifulSoup
import requests
DATA = 'RY.csv'
URL_BASE = 'https://ca.finance.yahoo.com/quote/'
HEADERS = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'}
# Load your data
def importData(path=DATA):
    df = pd.read_csv(path)
    df['date'] = pd.to_datetime(df['date'])
    df['day_of_week'] = df['date'].dt.dayofweek
    df['month'] = df['date'].dt.month
    df['year'] = df['date'].dt.year
    df['prev_close'] = df['close'].shift(1)
    df['next_close'] = df['close'].shift(-1)
    df = df.dropna()
    X = df[['open','high','low','close','prev_close','volume',
            'day_of_week','month','year']]
    y = df['next_close']
    return X, y


def getCurrentFeatures(stock = None):
    if not stock:
        stock = DATA.split('.')[0]
    url = URL_BASE+stock
    response = requests.get(url,HEADERS)
    soup = BeautifulSoup(response.text,'html.parser')
    data = soup.find_all('li',class_='yf-1qull9i')
    for i in data:
        print()#TODO:find features and compile into a df or something
