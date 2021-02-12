notouch = '#==make touch overlays go away (for switch)==\n\tself.get_node("GUI/TouchScreenButton").visible = false\n\tself.get_node("GUI/TouchScreenButton2").visible = false\n\tself.get_node("GUI/TouchScreenButton3").visible = false\n\tself.get_node("GUI/TouchScreenButton4").visible = false\n\tself.get_node("GUI/TouchScreenButton5").visible = false\n\t#============================================'

file = open("game.gd", "r+")

i = 0
lines = file.readlines()
for i in range(len(lines)):
    lines[i] = lines[i].replace("OS.get_system_time_secs()",
                                "SwitchTimer.get_estimate_time_secs")
    lines[i] = lines[i].replace("OS.get_system_time_msecs()",
                                "SwitchTimer.get_estimate_time_msecs")
    lines[i] = lines[i].replace("#NOTOUCH#",
                                notouch)
file.close()

ofile = open("game.gd", "w+")
i = 0
for i in range(len(lines)):
    ofile.write(lines[i])

ofile.close()
input("Done! Press any key to quit")
