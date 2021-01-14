extends Control

func _input(_event):
	if $quit_button.pressed==true:
		get_tree().quit()
	if $start_button.pressed==true:
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/title_menu.tscn")
