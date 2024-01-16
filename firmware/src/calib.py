import numpy as np

data = np.array(np.loadtxt('calib'))
data = data.T
print(np.mean(data[0]/data[1]))
