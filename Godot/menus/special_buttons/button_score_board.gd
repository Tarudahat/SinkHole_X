extends TextureButton

func _process(_delta):
	if self.pressed==true:
		get_tree().change_scene("res://menus/scores.tscn")
