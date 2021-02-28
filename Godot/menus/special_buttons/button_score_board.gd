extends TextureButton

func _process(_delta):
	if self.pressed==true:
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/scores.tscn")
