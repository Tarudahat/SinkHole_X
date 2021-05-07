extends Button

func _pressed():
	AudioPlayer.get_node("cancel_sfx").play()
	
