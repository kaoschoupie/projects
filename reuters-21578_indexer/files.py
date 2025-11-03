import os
from bs4 import BeautifulSoup
from nltk.tokenize import word_tokenize
import re
direc = os.path.dirname(__file__)
data_path = os.path.join(direc, 'reut')


def getFiles()-> list:
    """
    Open every file in the folder /reut in which all reuters21578 files are stored
    :return: a list of strings where each entry is the file
    """
    data = []
    for filename in os.listdir(data_path):#open every file in folder /reut
        with open(os.path.join(data_path, filename), 'r') as f:
            data.append(f.read())
    return data

def preprocessFile(file)-> list:
    """
    Preprocess a file by extracting the id and text of each article
    :param file: file to be preprocessed as a string
    :return: a list of tuples (id, text)
    """
    soup = BeautifulSoup(file, 'html.parser')
    metas = soup.find_all('reuters')
    ids = [meta['newid'] for meta in metas]
    articles = soup.find_all('text')
    texts = [str(article) for article in articles]
    counter = 0
    for i in texts:
        #r1 = r'<dateline>.*?</dateline>' #removes the entire dateline, commented out as I'm not sure if I wanna keep it
        r2 = r'</?body>|</?text>|</?title>|</?dateline>'#remove all tags
        #r3 = r'</.+>'
        r4 = r'(\\\d)'
        #i = re.sub(r1, '', i)
        i = re.sub(r2, ' ', i)
        #i = re.sub(r3,' ', i)
        i = re.sub(r4,' ', i)
        texts[counter] = i
        counter += 1
    #print(texts[0])
    output = list(zip(ids, texts))#make the pairs
    return output

#accessing an element of a tuple inside a list: nameofthelis[tupleindex][elementindex]
def tokenizeFiles(files) -> list:
    """
    Tokenize the body of each article in each file
    :param files: list of files to be tokenized
    :return: a list of tuples (id, [list of words])"""
    firstList = []
    for f in files:
        data = preprocessFile(f)
        for i in data:
            trans = re.sub(r'[^\w\s]', ' ', i[1])#remove any and all punctuation
            words = word_tokenize(trans)
            firstList.append((i[0], words))
    return firstList