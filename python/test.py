import os

inputlist=["toto"+str(i)+".root" for i in range(0,10)]
mystr=""
for i in inputlist:
    mystr+=i+" "

histoList="h1_Toto0 h2_Toto1 hToto2 hToto3 "
mystr+=histoList

outputFileName="superToto.root"

print "./test "+mystr
os.system("./bin/mergeRootFiles "+mystr)
os.system("mv output.root "+outputFileName)
