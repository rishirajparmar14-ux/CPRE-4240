def plot_bessel():
    # First, figure out how many points there are
    fid = open('bessel.data', 'r')
    NumPts = 0;
    while True:
        line = fid.readline()
        if not line: break
        NumPts = NumPts+1
    fid.close()

    import numpy as np
    import matplotlib.pyplot as plt

    # Second, read-in all the floats
    x  = np.zeros(NumPts,dtype=float)
    J0 = np.zeros(NumPts,dtype=float)
    fid = open('bessel.data', 'r')
    for k in range(0,NumPts):
        linestring = fid.readline()
        linelist   = linestring.split()
        x[k]       = float(linelist[0])
        J0[k]      = float(linelist[1])
    fid.close()

    # Third, plot the result
    plt.rc("font", size=16);
    plt.figure(figsize=(12,6))
    plt.plot(x,J0,color='b',linewidth=2,label=r'$J_0(x)$ (adaptive quadrature)');
    plt.plot(x,0.0*x,'k--',linewidth=1);
    plt.xlim(0.0,50.0)
    plt.ylim(-0.5,1.05)
    plt.xticks(np.linspace(0.0,50.0,11))
    plt.xlabel(r'$x$')
    plt.ylabel(r'$J_0(x)$')
    plt.title(r'$J_0(x)=\frac{1}{\pi}\int_0^\pi \cos(x\sin\theta)\,d\theta$')
    plt.legend(loc='upper right')
    plt.grid(True,alpha=0.3)
    plt.savefig('bessel_J0.png', dpi=200, bbox_inches='tight')
    plt.show();

if __name__ == "__main__":
    plot_bessel();
