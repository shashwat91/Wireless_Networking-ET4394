import scipy
data = scipy.fromfile(open('db_498'), dtype=scipy.float32)
file2 = open('db_values.txt','a')
print 'Length of data is %d' % (len(data))
avg=sum(data[-2000000:]) / float(len(data[-2000000:]))
print 'Average of last 2M samples are: ', avg
file2.writelines('Value for 498 MHz: '+ str(avg)+'\n')
