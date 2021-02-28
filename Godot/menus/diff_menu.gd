extends Node2D

onready var button_ok = get_node("background/button_ok")
onready var diff_name = get_node("background/diff_name")
onready var selected_diff=2
onready var change_info=true
onready var diffs=["Sloth","Normal","Hard","Impossible"]
 
func _ready():
	$background/arrow_buttons.input_array=diffs

func _input(_event):
	selected_diff=$background/arrow_buttons.selected_item
	$background/diff_name.text=diffs[selected_diff]
	if button_ok.pressed==true or Input.is_action_just_pressed("in_accept"):
		SceneGlobals.difficulty=selected_diff+1
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://game.tscn")
	elif $button_cancel.pressed==true or Input.is_action_just_pressed("ui_cancel"):
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/title_menu.tscn")
