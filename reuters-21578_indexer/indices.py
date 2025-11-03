from files import getFiles, tokenizeFiles
from nltk.stem import PorterStemmer
import re


def naiveIndex(tokens)->dict:
    """
    Create an inverted index using the naive method
    :param tokens: a list of tuples (id, [list of words])
    :return: the inverted index as a dictionary
    """
    f = []
    for i in tokens:
        for word in i[1]:
            f.append((word, i[0]))#term-id pair
    f = list(dict.fromkeys(f))#removes duplicates
    temp = []
    for i in f:
        temp.append((i[0], int(i[1])))#convert id to int for sorting
    temp.sort()#sorts by term, then by id
    table = dict()
    for i in temp:
        if(table.get(i[0]) is None):#if term is new, add term to index
            table[i[0]] = [i[1]]
        else:
            table[i[0]].append(i[1])#else append to term's postings list
            
    return table

def spimiIndex(tokens)->dict:
    """
    Create an inverted index using the spimi method
    :param tokens: a list of tuples (id, [list of words])
    :return: the inverted index as a dictionary
    """
    table  = dict()
    for i in tokens:
        for j in i[1]:
            if(table.get(j) is None):
                table[j] = [int(i[0])]
            else:
                table[j].append(int(i[0]))
            
    return table

def singleQuery(index, query)->str:
    """
    Process a single-word query
    :param index: the inverted index as a dictionary
    :param query: the query as a string
    :return: a list of document ids that contain the query word
    """
    #query = query.lower()
    if index.get(query) is not None:
        return index[query]
    else:
        return None

def andQuery(index, queryList)->list:
    """
    Process an AND query
    :param index: the inverted index as a dictionary
    :param queryList: the query as a list of strings containing strictly isolated words
    :return: a list of document ids that contain all the query words
    """
    result = None
    for query in queryList:
        #query = query.lower()
        if index.get(query) is not None:
            if result is None:
                result = set(index[query])
            else:
                result = result.intersection(set(index[query]))
        else:#if the term was in the query but not in the index at all
            return None
    
    return sorted(result)

def stopwords(index,size)->list:
    """
    This method compiles the n stopwords in a given index
    :param index: the index in need of a stopwords list
    :param size: the size of the stopwords list, usually 30 or 150
    :return: a list of strings as stopwords
    """
    words = []
    for i in index.items():
        postings = len(i[1])
        if len(words)<size:
            words.append((postings,i[0]))
        elif postings>words[-1][0]:
            words[-1]=((postings,i[0]))
        words.sort(reverse=True)


    with open('stopwords.txt', 'w') as f:
        for i in words:
            f.write(str(i) + '\n')
    output = [i[1] for i in words]
    return output


def calculateSize(index)->tuple:
    """
    Calculates the size of the term dictionary and the postings list
    :param index: the index in which we want to find the size
    :return: tuple(dictionary-size,postingslist-size)
    """
    terms = len(index)
    postings = 0
    for i in index.items():
        postings+=len(i[1])
    return terms,postings

def oneWordCompression(string,index)->str:
    """
    This method allows to 'compress' a word for compressed index query usage
    :param string: the word to be compressed
    :param index: the UNCOMPRESSED index
    :return: a string that was compressed the same as a dictionary term, may be None
    """
    if re.search(r"\d",string) is not None:
                return None
    string = string.lower()
    s30 = stopwords(index,30)
    s150 = stopwords(index,150)
    if string in s30:
        return None
    if string in s150:
        return None
    stemmer = PorterStemmer()
    string = stemmer.stem(string)
    return string

def compressionSteps(index):
    """
    Compresses the dictionary in steps, records the losses in copmression.txt
    No returns as it modifies the passed index
    :param index: the index to be compressed
    """
    with open('compression.txt','w') as f:
        total = calculateSize(index)
        current = total
        f.write('Initial size: ' + str(total[0]) + ' | postings: ' + str(total[1]) + '\n')
        f.write('The following numbers are the losses, not the new size\n')
        f.write('Numbers: terms: relative-loss total-loss | postings: relative-loss total-loss')
        stemmer = PorterStemmer()

        #remove numbers
        for i in list(index.keys()):
            if re.search(r"\d",i) is not None:
                del index[i]
        tmp = calculateSize(index)
        f.write('After \'no number\': ' + str(current[0]-tmp[0]) + ' ' + str(total[0]-tmp[0]) + ' | postings: ' + str(current[1]-tmp[1]) + ' ' + str(total[1]-tmp[1]) + '\n')
        current = tmp

        #case folding
        for i in list(index.keys()):
            temp1 = i.lower()
            if temp1==i:
                continue
            if temp1 in index:#if case folded term already exists
                temp2 = index[temp1] + index[i]#append postings list            
                temp2 = list(set(temp2))
                temp2.sort()
                index.update({temp1:temp2})
                #index[temp1] = temp2
                #list(set(index[temp1]))#remove duplicates and keep as list
                #index[temp1].sort()#sort postings list
            else:
                index[temp1] = index[i]
                #index[temp1] = i[1]
            del index[i]
        tmp = calculateSize(index)
        f.write('After \'case folding\': ' + str(current[0]-tmp[0]) + ' ' + str(total[0]-tmp[0]) + ' | postings: ' + str(current[1]-tmp[1]) + ' ' + str(total[1]-tmp[1]) + '\n')
        current = tmp
        
        s30 = stopwords(index,30)
        s150 = stopwords(index,150)

        #remove 30 stopwords
        for i in list(index.keys()):
            if i in s30:
                del index[i]
        tmp = calculateSize(index)
        f.write('After \'30 stopwords\': ' + str(current[0]-tmp[0]) + ' ' + str(total[0]-tmp[0]) + ' | postings: ' + str(current[1]-tmp[1]) + ' ' + str(total[1]-tmp[1]) + '\n')
        current = tmp

        #remove 150 stopwords
        for i in list(index.keys()):
            if i in s150:
                del index[i]
        tmp = calculateSize(index)
        f.write('After \'150 stopwords\': ' + str(current[0]-tmp[0]) + ' ' + str(total[0]-tmp[0]) + ' | postings: ' + str(current[1]-tmp[1]) + ' ' + str(total[1]-tmp[1]) + '\n')
        current = tmp

        #stem
        for i in list(index.keys()):
            temp1 = stemmer.stem(i)
            if temp1 == i:
                continue
            if temp1 in index:#if stem already exists
                temp2 = index[temp1] + index[i]#append postings list            
                temp2 = list(set(temp2))
                temp2.sort()
                index.update({temp1:temp2})
            else:
                index[temp1] = index[i]
            del index[i]          
        tmp = calculateSize(index)
        f.write('After \'stemming\': ' + str(current[0]-tmp[0]) + ' ' + str(total[0]-tmp[0]) + ' | postings: ' + str(current[1]-tmp[1]) + ' ' + str(total[1]-tmp[1]) + '\n')
        current = tmp
    #return index