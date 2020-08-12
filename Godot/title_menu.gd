extends Node2D

onready var button_left = get_node("background/button_left")
onready var button_right = get_node("background/button_right")
onready var button_ok = get_node("background/button_ok")
onready var thumbnail = get_node("background/thumbnail")
onready var level_name = get_node("background/level_name")
onready var selected_level=0
onready var change_info=false
onready var levels=["Plains","Highway","???","???","Random","???"]

func _process(_delta):
	if button_left.pressed==true or Input.is_action_pressed("in_left"):
		selected_level-=1
		change_info=true
	elif button_right.pressed==true or Input.is_action_pressed("in_right"):
		selected_level+=1
		change_info=true	

	if selected_level<0:
		selected_level=len(levels)-1
	elif selected_level>len(levels)-1:
		selected_level=0

	if change_info==true:
		thumbnail.texture=load("res://assets/thumbnails/thumbnail_"+str(selected_level)+".png")
		level_name.text=levels[selected_level]
		change_info=false
		OS.delay_msec(175)

	if button_ok.pressed==true or Input.is_action_pressed("ui_accept"):
		SceneGlobals.level=selected_level
		if levels[selected_level]=="Random":
			SceneGlobals.level=round(rand_range(0.8,1.8))-1
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://diff_menu.tscn")
		
	
