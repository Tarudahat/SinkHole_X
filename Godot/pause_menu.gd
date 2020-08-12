extends TextureRect

onready var button_continue = get_node("continue_button")
onready var button_quit = get_node("quit_button")


func _process(_delta):
	if button_quit.pressed==true:
		get_tree().paused=false
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://title_menu.tscn")
	elif button_continue.pressed==true:
		get_parent().visible=false
		get_tree().paused=false#
		
