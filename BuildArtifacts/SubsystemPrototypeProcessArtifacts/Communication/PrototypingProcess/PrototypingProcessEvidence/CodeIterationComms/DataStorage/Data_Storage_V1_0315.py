
'''--------------------------------------------------------------------------------------------
RECEIVING DATA FROM THE SENSORS
praxis III communications module
last update: 03/14/2024

this program will receive data from the pico that has information about the sensors
- receives the x and y coordinate of the plot it just sensed
- receives the nitrogen, phosphorous, potassium, and moisture values of the soil
  - WE DONT HAVE AN NPK SENSOR. SO WE CAN JUST PROXY IT AND USE RANDOMISED VALUES
  - just focus on moisture for the actual prototype
- based on the coordinates, store the NPKM values in the correct location in the 2D array


- we will have a parameter that will be nicely divisible so the farmers can choose which setting of how far apart the plots are
  - like 0.5, 0.25, 0.1, 0.05
- OKAY MAYBE NOT ANYMORE
- will receive position data as dead reckoning aka just position relative to the original
- so i can divide by the grid size to get an integer

- need to ask joey and andy about how they're going to know the plot size like are they also having an input...?
OK SO joey says just assume we have the info and we'll figure it out later
- all i need to ask for rn is sizing so i can parametrize but that's it.

NOTES
- add indonesian input sentences so that the farmers can understand (wow stakeholder analysis)

--------------------------------------------------------------------------------------------'''

#declaring imports
import numpy as np

from tkinter import ttk
import tkinter as tk
from tkinter import *
from tkinter.ttk import *
from tkinter import messagebox

import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
import seaborn as sns
import matplotlib.pylab as plt

'''DEF RECEIVE_DATA() FUNCTION----------------------------------------------------------------
- receive the x and y coordinates from the pico
- receive the NPK values from the pico
- NEED TO WAIT UNTIL I KNOW THE FORMAT OF THE DATA BEING SENT
--------------------------------------------------------------------------------------------'''






'''DEF MAKE_2D_ARRAY() FUNCTION----------------------------------------------------------------
- make a 2D array that will store the NPK values
- the size of the array will be determined by the parameter that the farmer chooses
- we will have "levels" of options for how far apart the plots are and they will choose the closest
- ask for input about plot x length and plot y length
like if they're 1m apart choose level 1, but if they're closer to 0.2m apart choose level 5

NOTE: these will become sliders on the GUI, i just left them for now so i can test
might do a step slider or a dropdown, probably just do set intervals though
--------------------------------------------------------------------------------------------'''
def make_2D_array():
    
    #ask for input about plot x length and plot y length
    plot_x_length = input("What is the length of the plot in the x direction? ")
    while plot_x_length.isdigit() == False:
        print("Please enter a valid number")
        plot_x_length = input("What is the length of the plot in the x direction? ")
    plot_y_length = input("What is the length of the plot in the y direction? ")
    while plot_y_length.isdigit() == False:
        print("Please enter a valid number")
        plot_y_length = input("What is the length of the plot in the y direction? ")
    
    #ask for input about how far apart the plots are
    plot_separation = input("Approximately how far are the plots? 1m, 0.5m? Please input the number only.")
    while plot_separation != "1" and plot_separation != "0.5": #can add more later idk
        print("Please enter a valid plot separation")
        plot_separation = input("Select one of these values: 1, 0.5")
    
    normalized_plot_x_length = int(int(plot_x_length) / float(plot_separation))
    normalized_plot_y_length = int(int(plot_y_length) / float(plot_separation))
        
    #create the 2D array
    data_array = np.zeros((normalized_plot_y_length + 1, normalized_plot_x_length + 1, 4))
    #there is a +1 because the array starts at 0, so for example 1m apart for 2m length will be 3 plots (0, 1, 2)
    return data_array, plot_separation, plot_x_length, plot_y_length


def get_x_input(text_box):
    x_value = text_box.get("1.0","end-1c")
    return x_value

def get_y_input(text_box1):
    y_value = text_box1.get("1.0","end-1c")
    return y_value
    

def test_make():
    window = tk.Tk()
    tk.Label(window, text="What is the length of the plot in the x direction?").pack()
    e1 = tk.Entry(window)
    text_box=Text(window, height=5, width=10)
    text_box.pack()
    
    submit = Button(window, text="Comment", command=lambda: get_x_input(text_box))
    submit.pack()
    
    tk.Label(window, text="What is the length of the plot in the y direction?").pack()
    e2 = tk.Entry(window)
    text_box=Text(window, height=5, width=10)
    text_box.pack()
    
    submit = Button(window, text="Comment", command=lambda: get_y_input(text_box))
    submit.pack()
    
    window.mainloop()
    return None #THIS DOESNT WORK IT DOES NOT GET STORED HERE??
#do i have to make it not a function??


#need to figure out how data will be sent to me. is it a file?
#need to figure out how the sensor will identify which plot it is, one at a time?
#like how will i know which plot is associated with which sensor info
'''READ DATA FILE FOR LOCATION FROM PICO------------------------------------------------------
- read the data file that the pico sends
- the data file will have the x and y coordinates of the plot
--------------------------------------------------------------------------------------------'''
def read_location_file(filename, sep, array):
    #file.open("data.txt", "r")
    #data = file.read()
    #file.close()
    return None

'''READ DATA FILE FOR SENSORS FROM PICO------------------------------------------------------
- read the data file that the pico sends
- the data file will have the sensor values of each plot
--------------------------------------------------------------------------------------------'''
def read_sensor_file(filename, sep, array):
    #file.open("data.txt", "r")
    #data = file.read()
    #file.close()
    return None



'''
how will i repeatedly read files and the sensor data?
can i assume that the info will be in the same order?
can i iteratively call the read line function?
maybe i need a larger function to call read loc, read sensor, place coordinates for every line
maybe i just read them in the same function
'''


'''DEF PLACE_COORDINATES(x, y) FUNCTION--------------------------------------------------------
- convert coordinates to the right index (divide by plot_separation)
- insert the NPK values into the 2D array
 [ [N, P, K], [N,P,K], [N,P,K], [N,P,K],
   [N, P, K], [N,P,K], [N,P,K], [N,P,K],
   [N, P, K], [N,P,K], [N,P,K], [N,P,K],
   [N, P, K], [N,P,K], [N,P,K], [N,P,K] ]
--------------------------------------------------------------------------------------------'''
def place_coordinates(x, y, sep, N, P, K, M, array):
    #assume i have been given everything in meters
    #the location is relative to top left corner as (0,0)
    #if i divide the location by the distance between the plots, i will get the index
    #use converted x and y to index into the function
    
    converted_x = int(int(x) / float(sep))
    converted_y = int(int(y) / float(sep))
    array[converted_y][converted_x] = [N, P, K, M]
    
    return None



''' MAKING THE GUI FUNCTION-------------------------------------------------------------------
- using tkinter to make the GUI
- will have sliders for the plot x length, plot y length, and plot separation
- will have a button to submit the values (confirm)
- will produce a heatmap of the NPK values
- will continuously update values as new NPK values are received
--------------------------------------------------------------------------------------------'''

''' GETTING NITROGEN VALUES FUNCTION-----------------------------------------------------------
get nitrogen values from the 3d array into a 2d array'''
def get_nitrogen(array):
    nitrogen_array = np.zeros([len(array), len(array[0])])
    for i in range(len(array)):
        for j in range(len(array[i])):
            nitrogen_array[i][j] = array[i][j][0]
    return nitrogen_array

''' GETTING PHOSPHOROUS VALUES FUNCTION-----------------------------------------------------------
get phosphorous values from the 3d array into a 2d array'''
def get_phosphorous(array):
    phosphorous_array = np.zeros([len(array), len(array[0])])
    for i in range(len(array)):
        for j in range(len(array[i])):
            phosphorous_array[i][j] = array[i][j][1]
    return phosphorous_array

''' GETTING POTASSIUM VALUES FUNCTION-----------------------------------------------------------
get potassium values from the 3d array into a 2d array'''
def get_potassium(array):
    potassium_array = np.zeros([len(array), len(array[0])])
    for i in range(len(array)):
        for j in range(len(array[i])):
            potassium_array[i][j] = array[i][j][2]
    return potassium_array

''' GETTING MOISTURE VALUES FUNCTION-----------------------------------------------------------
get moisture values from the 3d array into a 2d array'''
def get_moisture(array):
    moisture_array = np.zeros([len(array), len(array[0])])
    for i in range(len(array)):
        for j in range(len(array[i])):
            moisture_array[i][j] = array[i][j][3]
    return moisture_array

def make_gui(array, x, y):
    
    nitrogen_array = get_nitrogen(array)
    phosphorous_array = get_phosphorous(array)
    potassium_array = get_potassium(array)
    moisture_array = get_moisture(array)

    plt.subplot(2,2,1)
    heat_map = sns.heatmap(nitrogen_array, linewidth = 1 , annot = True, cmap = 'rainbow')
    #plt.pcolormesh(nitrogen_array, cmap = 'rainbow')
    plt.title('Nitrogen')
    
    plt.subplot(2,2,2)
    heat_map = sns.heatmap(phosphorous_array, linewidth = 1 , annot = True, cmap = 'rainbow')
    plt.title('Phosphorous')
    
    plt.subplot(2,2,3)
    heat_map = sns.heatmap(potassium_array, linewidth = 1 , annot = True, cmap = 'rainbow')
    plt.title('Potassium')
    
    plt.subplot(2,2,4)
    heat_map = sns.heatmap(moisture_array, linewidth = 1 , annot = True, cmap = 'rainbow')
    plt.title('Moisture')
    
    # the sensor value description
    # 0  ~300     dry soil   (BAD)
    # 300~700     humid soil (OPTIMAL)
    # 700~950     in water   (BAD)
    # make a MOISTURE color bar that works based on this?
    
    plt.tight_layout()
    plt.show()
    
    
    return None

'''DEF MAIN() FUNCTION-------------------------------------------------------------------------'''
if __name__ == '__main__':
    # test make the 2D array
    
    #test_make()
    
    
    data_array, plot_separation, plot_x_length, plot_y_length = make_2D_array()
    place_coordinates(1, 1, plot_separation, 1, 2, 3, 4, data_array) #test
    place_coordinates(0, 1, plot_separation, 4, 3, 2, 1, data_array) #test
    place_coordinates(1, 0, plot_separation, 2, 2, 2, 3, data_array) #test
    place_coordinates(0, 0, plot_separation, 3, 1, 4, 2, data_array) #test
    print(data_array)
    make_gui(data_array, plot_x_length, plot_y_length)
    
    
    

