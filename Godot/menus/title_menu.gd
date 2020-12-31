extends Node2D

onready var button_left = get_node("background/button_left")
onready var button_right = get_node("background/button_right")
onready var button_ok = get_node("background/button_ok")
onready var thumbnail = get_node("background/thumbnail")
onready var level_name = get_node("background/level_name")
onready var selected_level=0
onready var change_info=false
onready var levels=["Plains","Highway","Snow field","Volcano","Phantom field","Random","Custom"]
onready var prices=[0,150000,300000,450000,600000,0,1000000]
onready var done = false

func _input(event):
	
	if (button_left.pressed==true and (event is InputEventMouseButton )) or (Input.is_action_just_released("in_left")==true) :
		selected_level-=1
		change_info=true
	elif (button_right.pressed==true and (event is InputEventMouseButton)) or (Input.is_action_just_released("in_right")==true) :
		selected_level+=1
		change_info=true	
	if selected_level<0:
		selected_level=len(levels)-1
	elif selected_level>len(levels)-1:
		selected_level=0

	if change_info==true:
		if SceneGlobals.total_score>=prices[selected_level]:
			thumbnail.texture=load("res://assets/thumbnails/thumbnail_"+str(selected_level)+".png")
		else:
			thumbnail.texture=load("res://assets/thumbnails/thumbnail_"+str(selected_level)+"_locked.png")
		level_name.text=levels[selected_level]
		change_info=false

	
	if (button_ok.pressed==true or Input.is_action_just_pressed("in_accept")) and SceneGlobals.total_score>=prices[selected_level]:
		var max_lvl=0

		if levels[selected_level]=="Random":
			for i in range(len(levels)-1):
				if SceneGlobals.total_score>=prices[i] && levels[i]!="Random" && levels[i]!="Plains":
					max_lvl+=1

			selected_level=abs(round(rand_range(0,max_lvl)))

		SceneGlobals.level=selected_level
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/diff_menu.tscn")
	else:
		pass
		#   X sfx


		
	
