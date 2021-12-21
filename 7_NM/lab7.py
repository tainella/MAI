import numpy as np
import math
import matplotlib.pyplot as plt
from math import sqrt
from tkinter import *
from PIL import Image, ImageTk

def stop(L,U,x,y):
    maxx = 0
    for i in range(len(x)):
        for j in range(len(y)):
            if abs(U[i,j] - L[i,j]) > maxx:
                maxx = abs(U[i,j] - L[i,j])
    return maxx

def norma(a):
    norm = 0
    for i in range(len(a)):
        norm += a[i]**2
    return sqrt(norm)

def f(x,y):
    return 0

def phi1(y):
    return np.exp(-y)*np.cos(y)

def phi2(y):
    return 0

def phi3(x):
    return np.exp(-x)*np.cos(x)

def phi4(x):
    return 0

def true_fval(x, y):
    return np.exp(- x - y)*np.cos(x)*np.cos(y)

d = 1
a = 2
b = 2
c = 4

alfa1 = 0
alfa2 = 1
beta1 = 0
beta2 = 1
gama1 = 0
gama2 = 1
delta1 = 0
delta2 = 1

lbx = 0
ubx = np.pi/2
nx = 30
hx = (ubx - lbx)/nx


lby = 0
uby = np.pi/2
ny = 30
hy = (uby - lby)/ny


x = np.arange(lbx, ubx + hx, hx)
y = np.arange(lby, uby + hy, hy)

#print(x,'\n',y)

eps = 0.001

def print_matr(U,x,y):
    print('            ', end = ' ')
    for j in range(len(x)):
        print('x = ', "%.2f" %x[j], end = '  ')
    print(' ')
    for i in range(len(y)):
        print('y = ',"%.3f" % y[i], end = ' |')
        for j in range(len(x)):
            print("%.5f" % U[j,i], end = '  ')
        print(' ')
    return

def copy_matr(U,x,y):
    L = np.zeros((len(x),len(y)))
    for i in range(len(x)):
        for j in range(len(y)):
            L[i,j] = U[i,j]
    return L

def MPI(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby):
    x = np.arange(lbx, ubx + hx, hx)
    y = np.arange(lby, uby + hy, hy)
    U = np.zeros((len(x),len(y)))
    for i in range(len(x)):
        U[i,0] = phi3(x[i])/gama2
        U[i,-1] = phi4(x[i])/delta2    
    for j in range(len(y)):
        U[0,j] = phi1(y[j])/alfa2
        U[-1,j] = phi2(y[j])/beta2
    k = 0
    for j in range(1,len(y)-1):
        for i in range(1,len(x)-1):
            U[i,j] = U[0,j] + (x[i] - x[0])*(U[-1,j] - U[0,j])/(x[-1] - x[0])           
    while True:
        k = k+1
        #print('k = ', k, end = '    ')
        L = copy_matr(U,x,y)
        U = np.zeros((len(x),len(y)))
        for i in range(len(x)):
            U[i,0] = phi3(x[i])/gama2
            U[i,-1] = phi3(x[i])/delta2    
        for j in range(1, len(y)-1):
            U[0,j] = phi1(y[j])/alfa2
            U[-1,j] = phi2(y[j])/beta2
        
        for j in range(1, len(y)- 1):
            for i in range(1, len(x)- 1):
                U[i,j] = (hx*hx*f(x[i],y[j]) - (L[i+1,j] + L[i-1,j]) - d*hx*hx*(L[i,j+1] + L[i,j-1])/(hy*hy) - a*hx*0.5*(L[i+1,j] - L[i-1,j]) - b*hx*hx*(L[i,j+1] - L[i,j-1])/(2*hy) )/(c*hx*hx - 2*(hy*hy + d*hx*hx)/(hy*hy))

        #print(' eps = ',stop(L,U,x,y))
        if stop(L,U,x,y) <= eps:
            break
    return U, k


def ZEI(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby):
    x = np.arange(lbx, ubx + hx, hx)
    y = np.arange(lby, uby + hy, hy)
    U = np.zeros((len(x),len(y)))
    for i in range(len(x)):
        U[i,0] = phi3(x[i])/gama2
        U[i,-1] = phi4(x[i])/delta2    
    for j in range(1,len(y)-1):
        U[0,j] = phi1(y[j])/alfa2
        U[-1,j] = phi2(y[j])/beta2
    k = 0
    for j in range(1,len(y)-1):
        for i in range(1,len(x)-1):
            U[i,j] = U[0,j] + (x[i] - x[0])*(U[-1,j] - U[0,j])/(x[-1] - x[0])           
    while True:
        k = k+1
        #print('k = ', k, end = '    ')
        L = copy_matr(U,x,y)
        U = np.zeros((len(x),len(y)))
        for i in range(len(x)):
            U[i,0] = phi3(x[i])/gama2
            U[i,-1] = phi3(x[i])/delta2    
        for j in range(1, len(y)-1):
            U[0,j] = phi1(y[j])/alfa2
            U[-1,j] = phi2(y[j])/beta2
            
        for j in range(1, len(y)- 1):
            for i in range(1, len(x)- 1):
                U[i,j] = (hx*hx*f(x[i],y[j]) - (L[i+1,j] + U[i-1,j]) - d*hx*hx*(L[i,j+1] + U[i,j-1])/(hy*hy) - a*hx*0.5*(L[i+1,j] - U[i-1,j]) - b*hx*hx*(L[i,j+1] - U[i,j-1])/(2*hy) )/(c*hx*hx - 2*(hy*hy + d*hx*hx)/(hy*hy))

        #print(' eps = ',stop(L,U,x,y))
        if stop(L,U,x,y) <= eps:
            break
    return U, k


def MPIR(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby, w):
    x = np.arange(lbx, ubx + hx, hx)
    y = np.arange(lby, uby + hy, hy)
    U = np.zeros((len(x),len(y)))
    for i in range(len(x)):
        U[i,0] = phi3(x[i])/gama2
        U[i,-1] = phi4(x[i])/delta2    
    for j in range(1,len(y)-1):
        U[0,j] = phi1(y[j])/alfa2
        U[-1,j] = phi2(y[j])/beta2
    k = 0
    for j in range(1,len(y)-1):
        for i in range(1,len(x)-1):
            U[i,j] = U[0,j] + (x[i] - x[0])*(U[-1,j] - U[0,j])/(x[-1] - x[0])           
    while True:
        k = k+1
        #print('k = ', k, end = '    ')
        L = copy_matr(U,x,y)
        U = np.zeros((len(x),len(y)))
        for i in range(len(x)):
            U[i,0] = phi3(x[i])/gama2
            U[i,-1] = phi3(x[i])/delta2    
        for j in range(1, len(y)-1):
            U[0,j] = phi1(y[j])/alfa2
            U[-1,j] = phi2(y[j])/beta2
            
        for j in range(1, len(y)- 1):
            for i in range(1, len(x)- 1):
                U[i,j] = ((hx*hx*f(x[i],y[j]) - (L[i+1,j] + U[i-1,j]) - d*hx*hx*(L[i,j+1] + U[i,j-1])/(hy*hy) - a*hx*0.5*(L[i+1,j] - U[i-1,j]) - b*hx*hx*(L[i,j+1] - U[i,j-1])/(2*hy) )/(c*hx*hx - 2*(hy*hy + d*hx*hx)/(hy*hy)))*w + (1 - w)*L[i,j]

        #print(' eps = ',stop(L,U,x,y))
        if stop(L,U,x,y) <= eps:
            break
    return U, k


def plotU(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby, w, ky):
    x = np.arange(lbx, ubx + hx, hx)
    y = np.arange(lby, uby + hy, hy)
    U1, k1 = MPI(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby)
    U2, k2 = ZEI(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby)
    U3, k3 = MPIR(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby, w)
    plt.figure(1)
    tit = 'U(x, ' + str(ky) + ')'
    plt.title(tit)
    label1 = 'МПИ (количество итераций: ' + str(k1) + ')'
    label2 = 'метод Зейделя (количество итераций: ' + str(k2) + ' )'
    label3 = 'МПИ с верхней релаксацией (количество итераций: ' + str(k3) + ')'
    plt.plot(x,true_fval(x,y[ky]),color = 'red', label = 'аналитическое решение')
    plt.plot(x,U1[:,ky],color = 'green', label = label1)
    plt.plot(x,U2[:,ky],color = 'blue', label = label2)
    plt.plot(x,U3[:,ky],color = 'orange', label = label3)
    plt.legend()
    plt.grid()
    
    
    plt.figure(3)
    plt.title('Погрешность')
    plt.xlim((0, 1.4))
    plt.grid()
    eps = []
    for i in range(len(x)):
        a = abs(true_fval(x[i], y[ky]) - U1[i,ky])
        eps = np.append(eps, a)
    plt.plot(x, eps, color = 'green')

    eps = []
    for i in range(len(x)):
        a = abs(true_fval(x[i], y[ky]) - U2[i,ky])
        eps = np.append(eps, a)
    plt.plot(x, eps, color = 'blue')
    
    eps = []
    for i in range(len(x)):
        a = abs(true_fval(x[i], y[ky]) - U3[i,ky])
        eps = np.append(eps, a)
    plt.plot(x, eps, color = 'orange')
    plt.show()    
    return

root = Tk()
root.title("Лабораторная работа №7")
root["bg"] = "grey"
w = root.winfo_screenwidth() # ширина экрана
h = root.winfo_screenheight() # высота экрана
ww = str(int(w/2))
hh = str(int(h-100))
a = ww + 'x' + hh
w = w//2 # середина экрана
h = h//2 
w = w - 200 # смещение от середины
h = h - 200
root.geometry(a + '+0+0'.format(w, h))

label4 = Label(text = "Параметры задачи:", justify = LEFT, font = "Arial 12")
label4.place(x = 10, y = 10)

labela = Label(text = "a = ", justify = LEFT, font = "Arial 12", bg = "grey")
labela.place(x = 10, y = 40)
labelb = Label(text = "b = ", justify = LEFT, font = "Arial 12", bg = "grey")
labelb.place(x = 10, y = 70)
labelc = Label(text = "c = ", justify = LEFT, font = "Arial 12", bg = "grey")
labelc.place(x = 10, y = 100)
labeld = Label(text = "d = ", justify = LEFT, font = "Arial 12", bg = "grey")
labeld.place(x = 10, y = 130)
labeleps = Label(text = "eps = ", justify = LEFT, font = "Arial 10", bg = "grey")
labeleps.place(x = 10, y = 160)
labelw = Label(text = "w = ", justify = LEFT, font = "Arial 10", bg = "grey")
labelw.place(x = 10, y = 190)


entrya = Entry(root, justify = RIGHT)
entrya.place(x = 30, y = 40)

entryb = Entry(root, justify = RIGHT)
entryb.place(x = 30, y = 70)

entryc = Entry(root, justify = RIGHT)
entryc.place(x = 30, y = 100)

entryd = Entry(root, justify = RIGHT)
entryd.place(x = 30, y = 130)

entryeps = Entry(root, justify = RIGHT)
entryeps.place(x = 30, y = 160)

entryw = Entry(root, justify = RIGHT)
entryw.place(x = 30, y = 190)

entrya.insert(0, 2)
entryb.insert(0, 2)
entryc.insert(0, 4)
entryd.insert(0, 1)
entryeps.insert(0, 0.001)
entryw.insert(0, 1.5)

label5 = Label(text = "Параметры конечно-разностной сетки:", justify = LEFT, font = "Arial 12")
label5.place(x = 300, y = 10)
labellx = Label(text = "lx = ", justify = LEFT, font = "Arial 12", bg = "grey")
labellx.place(x = 300, y = 40)
labelnx = Label(text = "nx = ", justify = LEFT, font = "Arial 12", bg = "grey")
labelnx.place(x = 300, y = 70)
labelly = Label(text = "ly = ", justify = LEFT, font = "Arial 12", bg = "grey")
labelly.place(x = 300, y = 100)
labelny = Label(text = "ny = ", justify = LEFT, font = "Arial 12", bg = "grey")
labelny.place(x = 300, y = 130)

entrylx = Entry(root, justify = RIGHT)
entrylx.place(x = 330, y = 40)

entrynx = Entry(root, justify = RIGHT)
entrynx.place(x = 330, y = 70)

entryly = Entry(root, justify = RIGHT)
entryly.place(x = 330, y = 100)

entryny = Entry(root, justify = RIGHT)
entryny.place(x = 330, y = 130)

entrylx.insert(0, np.pi/2)
entrynx.insert(0, 30)
entryly.insert(0, np.pi/2)
entryny.insert(0, 30)

labelhx = Label(text = " ", justify = LEFT, font = "Arial 12", bg = "grey")
labelhx.place(x = 650, y = 40)
labelhy = Label(text = " ", justify = LEFT, font = "Arial 12", bg = "grey")
labelhy.place(x = 650, y = 70)
labelerror = Label(text = " ", justify = LEFT, font = "Arial 12", bg = "grey")
labelerror.place(x = 650, y = 100)


def params():
    try:
        lbx = 0
        ubx = float(entrylx.get())
        lby = 0
        uby = float(entryly.get())
        nx = float(entrynx.get())
        ny = float(entryny.get())
        labelhx.config(text = "hx = {}".format((ubx - lbx)/nx))
        labelhy.config(text = "hy = {}".format((uby - lby)/ny))
    except ValueError:
        labelerror.config(text = "Заполните все поля", fg = "red")

but = Button(root, text = "Показать все параметры сетки", command = params, font = "Arial 9")
but.place(x = 650, y = 10)

labelky = Label(text = "ky = ", justify = LEFT, font = "Arial 10", bg = "grey")
labelky.place(x = 10, y = 250)

entryky = Entry(root, justify = RIGHT)
entryky.place(x = 30, y = 250)
entryky.insert(0, 1)

ky = float(entryky.get())

d = float(entryd.get())
a = float(entrya.get())
b = float(entryb.get())
c = float(entryc.get())

alfa1 = 0
alfa2 = 1
beta1 = 0
beta2 = 1
gama1 = 0
gama2 = 1
delta1 = 0
delta2 = 1

lbx = 0
ubx = float(entrylx.get())
nx = float(entrynx.get())
hx = (ubx - lbx)/nx


lby = 0
uby = float(entryly.get())
ny = float(entryny.get())
hy = (uby - lby)/ny

eps = float(entryeps.get())
w = float(entryw.get())

        

def solvv():
    try:
        ky = int(entryky.get())
        d = float(entryd.get())
        a = float(entrya.get())
        b = float(entryb.get())
        c = float(entryc.get())

        alfa1 = 0
        alfa2 = 1
        beta1 = 0
        beta2 = 1
        gama1 = 0
        gama2 = 1
        delta1 = 0
        delta2 = 1

        lbx = 0
        ubx = float(entrylx.get())  
        nx = float(entrynx.get())
        hx = (ubx - lbx)/nx

        lby = 0
        uby = float(entryly.get())
        ny = float(entryny.get())
        hy = (uby - lby)/ny

        eps = float(entryeps.get())
        w = float(entryw.get())
        plotU(a, b, c, d, alfa1, alfa2, beta1, beta2, gama1, gama2, delta1, delta2, hx, hy, eps, lbx, lby, ubx, uby, w, ky)
    except ValueError:
        labelerror.config(text = "Заполните все поля", fg = "red")



solv = Button(root, text = "  Решить  ", font = "Arial 14", command = solvv)
solv.place(x = 650, y = 250)

root.mainloop()
