from indices import *
from files import getFiles, tokenizeFiles
import time
def main():
    print('Starting process')
    files = getFiles()
    tokenList = tokenizeFiles(files)
    print('Files have been preprocessed and tokenized')
    value = 1

    index1 = naiveIndex(tokenList)
    print('Naive index was compiled')
    challenges(tokenList)
    while value!=0:
        print('Welcome to the demo file, please enter one of the following options:\n')
        #print('1: Subproject 1: create a naive index\n')
        print("2: Subproject 2: use naive index and run the sample queries\n")
        print('3: Subproject 3: create a naive index, compress it, run the sample queries\n')
        print('4: Subproject 4: create a spimi index, create a naive index, compare construction time for 10 000 term-docID pairs\n')
        print('0: exit this demo\n')
        print('Enter an option:')
        value = int(input())
        if value == 2:
            sub2(index1)
        elif value == 3:
            sub3(tokenList)
        elif value == 4:
            sub4(tokenList)
        #elif value == 0:
         #   asdf

def sub2(index):
    print('Enter 1 to enter some queries\n')
    print('Enter 2 to run the sample queries in samples.txt')
    value = int(input())

    #user queries
    if value == 1:
        print('Enter any query(either single, or using AND):')
        query = input()
        queryList = query.split(' AND ')
        if len(queryList)==1:
            print(str(singleQuery(index,queryList[0])) + '\n')
        else:
            print(str(andQuery(index,queryList))+ '\n')

    #sample queries
    elif value == 2:
         with open('samples.txt', 'r') as f:
            lines = f.readlines()

            #single term
            for i in range(3):
                q = singleQuery(index,lines[i].rstrip())
                print('Query: ' + lines[i] + ' : ' + str(q) + '\n')
            
            #AND
            for i in range(3,6):
                queryList = lines[i].rstrip().split(' AND ')
                q = andQuery(index,queryList)
                print('Query: ' + lines[i] + ' : ' + str(q) + '\n')

def sub3(tokenList):
    index = naiveIndex(tokenList)
    compressionSteps(index)
    with open('samples.txt', 'r') as f:
        lines = f.readlines()

        #single term
        for i in range(3):
            cc = oneWordCompression(lines[i].rstrip(),index)#compress the word
            q = singleQuery(index,cc)
            print('Query: ' + lines[i] + ' : ' + str(q) + '\n')
        
        #AND
        for i in range(3,6):
            queryList = lines[i].rstrip().split(' AND ')
            ccList = []

            #compress each word separately
            for j in queryList:
                ccList.append(oneWordCompression(j,index))
            q = andQuery(index,ccList)
            print('Query: ' + lines[i] + ' : ' + str(q) + '\n')

def sub4(tokenList):
    miniList = []
    counter = 0
    #make the 10000 pairs from the total pairs
    for i in tokenList:
        if(len(i[1])+counter)>10000:
            newPair = (i[0],i[1][:(10000-counter)])
            miniList.append(newPair)
            break
        else:
            miniList.append(i)
            counter+=len(i[1])
    #create and measure
    naiveStart = time.time()
    naiveOne = naiveIndex(miniList)
    naiveEnd = time.time()
    spimiStart = time.time()
    spimiOne = spimiIndex(miniList)
    spimiEnd = time.time()
    naiveTime = naiveEnd - naiveStart
    spimiTime = spimiEnd - spimiStart
    print('Time to create the naive index based on 10000 pairs is ' + str(naiveTime) + 's and the time to create the spimi index is ' + str(spimiTime) + 's.\n')

def challenges(tokenList):
    naiveOne = naiveIndex(tokenList)
    spimiOne = spimiIndex(tokenList)
    with open('challengeruns.txt','w') as f:
        start = time.time()
        f.write(str(andQuery(naiveOne,['Chrysler','Bundesbank'])))
        stop = time.time()
        f.write(str(stop-start) + 's for naive index to run Chrysler AND Bundesbank\n')
        start = time.time()
        f.write(str(singleQuery(naiveOne,'pineapple')))
        stop = time.time()
        f.write(str(stop-start) + 's for naive index to run pineapple\n')

        start = time.time()
        f.write(str(andQuery(spimiOne,['Chrysler','Bundesbank'])))
        stop = time.time()
        f.write(str(stop-start) + 's for spimi index to run Chrysler AND Bundesbank\n')
        start = time.time()
        f.write(str(singleQuery(spimiOne,'pineapple')))
        stop = time.time()
        f.write(str(stop-start) + 's for spimi index to run pineapple\n')

        f.write('\n24H challenge runs complete. Time for the normal challenge runs.\n\n')

        start = time.time()
        f.write(str(singleQuery(naiveOne,'copper')))
        stop = time.time()
        f.write(str(stop-start) + 's for naive index to run copper\n')
        start = time.time()
        f.write(str(singleQuery(naiveOne,'Chrysler')))
        stop = time.time()
        f.write(str(stop-start) + 's for naive index to run Chrysler\n')
        start = time.time()
        f.write(str(singleQuery(naiveOne,'Bundesbank')))
        stop = time.time()
        f.write(str(stop-start) + 's for naive index to run Bundesbank\n')        

        start = time.time()
        f.write(str(singleQuery(spimiOne,'copper')))
        stop = time.time()
        f.write(str(stop-start) + 's for spimi index to run copper\n')
        start = time.time()
        f.write(str(singleQuery(spimiOne,'Chrysler')))
        stop = time.time()
        f.write(str(stop-start) + 's for spimi index to run Chrysler\n')
        start = time.time()
        f.write(str(singleQuery(spimiOne,'Bundesbank')))
        stop = time.time()
        f.write(str(stop-start) + 's for spimi index to run Bundesbank\n')    




if __name__=='__main__':
    main()