import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import scipy.stats as stats
import math

def func1():
     print("Hello test!")
     return 1;
 
def gaussian(x, *param):
    return (1/(math.sqrt(2*math.pi)*param[1])) * np.exp(-np.power(x - param[0], 2.) / (2 * np.power(param[1], 2.)))


def plot_histogram_():
    mu = 2
    variance = 0.1
    sigma = 8
    x = np.linspace(mu - 3*sigma, mu + 3*sigma, 100)
    plt.plot(x, stats.norm.pdf(x, mu, sigma))
    print("plot_histogram_!")
    plt.show()


# plot_histogram_()


