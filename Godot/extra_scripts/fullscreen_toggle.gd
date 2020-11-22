extends Node2D

var toggle=true

func _input(_event):
	if Input.is_action_just_pressed("f11"):
		if toggle==true:
			toggle=false
		else:
			toggle=true
	OS.window_fullscreen=toggle
	
