extends Node2D

onready var button_left = get_node("background/button_left")
onready var button_right = get_node("background/button_right")
onready var button_ok = get_node("background/button_ok")
onready var button_cancel = get_node("background/button_cancel")
onready var diff_name = get_node("background/diff_name")
onready var selected_diff=2
onready var change_info=true
onready var diffs=["Sloth","Normal","Hard","Impossible"]

func _input(event):
	
	if (button_left.pressed==true and (event is InputEventMouseButton )) or (Input.is_action_just_released("in_left")==true) :
		selected_diff-=1
		change_info=true
	elif (button_right.pressed==true and (event is InputEventMouseButton)) or (Input.is_action_just_released("in_right")==true) :
		selected_diff+=1
		change_info=true	

	if selected_diff<0:
		selected_diff=len(diffs)-1
	elif selected_diff>len(diffs)-1:
		selected_diff=0

	if change_info==true:
		diff_name.text=diffs[selected_diff]
		change_info=false

	if button_ok.pressed==true or Input.is_action_just_pressed("in_accept"):
		SceneGlobals.difficulty=selected_diff+1
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://game.tscn")
	elif button_cancel.pressed==true or Input.is_action_just_pressed("ui_cancel"):
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/title_menu.tscn")
		
	
