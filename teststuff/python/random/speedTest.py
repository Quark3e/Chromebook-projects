import time


testVal = 0

start_time = time.time()


for i in range(1000000):
    testVal=i
print(testVal)

print("time elapsed: ", time.time()-start_time, sep='')