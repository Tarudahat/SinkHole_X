import tkinter as tk
from tkinter import ttk, PhotoImage, Label

map_size_str = input("What dimensions should the map have? (x,y)\n")
map_size_arr_str = map_size_str.split(",")
map_size = [int(map_size_arr_str[0]), int(map_size_arr_str[1])]

win = tk.Tk()

win.title("Map maker")

# main widget
main_wid = ttk.Labelframe(win, text="Map:")
main_wid.grid(column=1, row=4+map_size[0], padx=16, pady=16)
img_wid = ttk.Labelframe(win, text="Img:")
img_wid.grid(column=1, row=6+map_size[0])

i = 0
tiles = []
# create text boxes
for i_y in range(map_size[1]):
    for i_x in range(map_size[0]):
        # make input box

        tiles.append(ttk.Entry(main_wid, width=3))
        tiles[i].grid(column=i_x, row=i_y)  # place it on it's position
        i += 1


def give_str():
    output_str = "{ {"
    i2 = 0
    for i in range(map_size[0]*map_size[1]):
        if i2 == map_size[0]:
            output_str += "},{"
            i2 = 0
        elif i2 >= 1:
            output_str += ","
        zero = ""
        if tiles[i].get() == "":
            zero = "0"

        output_str += tiles[i].get()+zero
        i2 += 1
    output_str += "} };"
    print(output_str)


img = PhotoImage(file="t.png")
img_widget = Label(img_wid, image=img)
img_widget.grid(column=0, row=0)


# the do a thing button
done_button = ttk.Button(main_wid, text="OK", command=give_str)
done_button.grid(column=1+map_size[0], row=2+map_size[1])

# run
win.mainloop()
