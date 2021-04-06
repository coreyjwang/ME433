#!/usr/bin/env python
# coding: utf-8

# # Import csv function

# In[5]:


def python_csv(csv_file):
    import csv

    t = [] # column 0
    data1 = [] # column 1
    with open(csv_file) as f:
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            data1.append(float(row[0])) # leftmost column
            t.append(float(row[1])) # second column
    return t, data1;


# ### Calculate sample rate function

# In[6]:


def calculate_sample_rate(time): # Time is output t from python_csv()
    #sample rate = number of data points / total time of samples
    sample_rate = len(time) / (time[-1] - time[0])
    return sample_rate


# In[358]:


calculate_sample_rate(python_csv('sigA.csv')[1])


# In[361]:


calculate_sample_rate(python_csv('sigB.csv')[1])


# In[362]:


calculate_sample_rate(python_csv('sigC.csv')[1])


# In[363]:


calculate_sample_rate(python_csv('sigD.csv')[1])


# # 4. FFT function

# In[7]:


def FFT2(s,t): # s = data list, t = time list
    import matplotlib.pyplot as plt
    import numpy as np

    Fs = calculate_sample_rate(t) # sample rate
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,y,'b')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'b') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()


# In[8]:


# CSV A
s,t = python_csv('sigA.csv')
FFT2(s,t)


# In[157]:


# CSV B
s,t = python_csv('sigB.csv')
FFT2(s,t)


# In[158]:


# CSV C
s,t = python_csv('sigC.csv')
FFT2(s,t)


# In[159]:


# CSV D
s,t = python_csv('sigD.csv')
FFT2(s,t)


# # 5. Moving average filter

# In[9]:


def MAV(data, time, x):
    import numpy as np
    
    data_length = len(data)
    data = (x * [0]) + data # Set first x values to 0
    for i in range(data_length):
        data[i] = sum(data[i:(i+x)])
    data_new = np.array(data) / x
    data_new = data_new.tolist()
    return data_new[x-1:-1], time


# In[10]:


def p5(csv_file,x):
    import matplotlib.pyplot as plt
    import numpy as np
    
    s,t = python_csv(csv_file)
    sa,ta = MAV(s,t,x)
    
    # FFT Plot
    Fs = calculate_sample_rate(t) # sample rate
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,y,'k')
    ax1.plot(t,sa,'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'k') # plotting the fft
    
    y = sa # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]
    
    ax2.loglog(frq,abs(Y),'r') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.suptitle(csv_file + ', ' + str(x) + ' points averaged')
    plt.show()


# ### Function to create plots for problem 5

# In[259]:


p5('sigA.csv',500)


# In[260]:


p5('sigB.csv',500)


# In[294]:


p5('sigC.csv',200)


# In[292]:


p5('sigD.csv',800)


# # 6. IIR Filter

# In[23]:


def IIR(data, time, A, B):
    import numpy as np
    
    data_length = len(data)
    data_new = [0] * data_length
    for i in range(1,data_length):
        data_new[i] = A * data_new[i-1] + B * data[i]
    return data_new, time


# In[24]:


def p6(csv_file,A,B):
    import matplotlib.pyplot as plt
    import numpy as np
    
    s,t = python_csv(csv_file)
    sa,ta = IIR(s,t,A,B)
    
    # FFT Plot
    Fs = calculate_sample_rate(t) # sample rate
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,y,'k')
    ax1.plot(t,sa,'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'k') # plotting the fft
    
    y = sa # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]
    
    ax2.loglog(frq,abs(Y),'r') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.suptitle(csv_file + ', ' + 'A = ' + str(A) + ', ' + 'B = ' + str(B))
    plt.show()


# In[28]:


p6('sigA.csv',0.999,0.001)


# In[30]:


p6('sigB.csv',0.999,0.001)


# In[34]:


p6('sigC.csv',0.995,0.005)


# In[36]:


p6('sigD.csv',0.999,0.001)


# # 7. FIR Filter

# In[132]:


# Old version that didn't work properly
def FIR_old(data, time, h):
    import numpy as np
    
    data_length = len(data)
    x = len(h)
    data = (x * [0]) + data # Set first x values to 0
    for i in range(data_length):
        for j in range(x):
            data[i] += h[j] * data[i+j]
    data_new = np.array(data)
    data_new = data_new.tolist()
    return data_new[x-1:-1], time


# In[133]:


# FIXED VERSION
def FIR(data, time, h):
    import numpy as np
    
#     data_length = len(data)
#     x = len(h)
    data_new = np.convolve(data, h, mode = "same")
    
#     data = (x * [0]) + data # Set first x values to 0
#     for i in range(x,data_length+x):
#         for j in range(x):
#             data[i] += h[j] * data[i-j]

    return data_new, time


# In[134]:


def p7(csv_file,h):
    import matplotlib.pyplot as plt
    import numpy as np
    
    s,t = python_csv(csv_file)
    sa,ta = FIR(s,t,h)
    
    # FFT Plot
    Fs = calculate_sample_rate(t) # sample rate
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,y,'k')
    ax1.plot(t,sa,'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'k') # plotting the fft
    
    y = sa # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]
    
    ax2.loglog(frq,abs(Y),'r') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.suptitle(csv_file + ', ' + 'f_l = 20 Hz, b_l = 50 Hz')
    plt.show()


# In[135]:


h = [
    -0.000604064461847955,
    -0.000551780069321844,
    -0.000507919670677880,
    -0.000454487696572236,
    -0.000369375235857037,
    -0.000227140994277224,
    0.000000000000000000,
    0.000341015939334090,
    0.000824770783395443,
    0.001478661559683630,
    0.002327237052419366,
    0.003390857033446438,
    0.004684451187085917,
    0.006216434312797693,
    0.007987829321982980,
    0.009991642156324147,
    0.012212523327184123,
    0.014626739684373311,
    0.017202467721139997,
    0.019900406721944881,
    0.022674696908148821,
    0.025474114994420292,
    0.028243507783721956,
    0.030925414113290178,
    0.033461817071338058,
    0.035795962308429512,
    0.037874174746726358,
    0.039647605214021407,
    0.041073840550354880,
    0.042118315486222989,
    0.042755471888911503,
    0.042969620523712268,
    0.042755471888911503,
    0.042118315486222989,
    0.041073840550354880,
    0.039647605214021414,
    0.037874174746726358,
    0.035795962308429519,
    0.033461817071338058,
    0.030925414113290181,
    0.028243507783721956,
    0.025474114994420295,
    0.022674696908148818,
    0.019900406721944885,
    0.017202467721140008,
    0.014626739684373313,
    0.012212523327184133,
    0.009991642156324143,
    0.007987829321982982,
    0.006216434312797696,
    0.004684451187085917,
    0.003390857033446442,
    0.002327237052419366,
    0.001478661559683631,
    0.000824770783395443,
    0.000341015939334090,
    0.000000000000000000,
    -0.000227140994277224,
    -0.000369375235857037,
    -0.000454487696572237,
    -0.000507919670677880,
    -0.000551780069321844,
    -0.000604064461847955,
]


# In[136]:


# CSV A
p7('sigA.csv',h)


# In[137]:


# CSV B
p7('sigB.csv',h)


# In[138]:


# CSV C
p7('sigC.csv',h)


# In[139]:


# CSV D
p7('sigD.csv',h)


# In[ ]:




