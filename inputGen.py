import random
import sys
orig_stdout = sys.stdout
for i in range(1,7):
  tot=10**i
  fname="input_"+str(tot)+".txt"
  f = open(fname, 'w')
  sys.stdout = f
  i=0
  number=ord('a')
  while(i!=tot):
    x=random.randint(1,10)
    if x!=10:
      a=random.randint(1,100)
      count=random.randint(1,20)
      s='#'
      for k in range(count):
        b=random.randint(0,25)
        s=s+chr(number+b)
      print s+" "+str(a)
    else:
      print(random.randint(1,20))
    i+=1
  print("stop")
  sys.stdout = orig_stdout
  f.close()
