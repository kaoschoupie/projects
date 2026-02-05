from utils import importData
from model import MultiLinearRegression

def main():
    x,y = importData()
    model = MultiLinearRegression()
    model.fit(x,y)
    

if __name__ == '__main__':
    main()