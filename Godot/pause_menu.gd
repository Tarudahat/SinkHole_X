extends TextureRect

onready var button_continue = get_node("continue_button")
onready var button_quit = get_node("quit_button")
onready var cursor = get_node("cursor")
onready var selected_button = 0


func _input(_event):
	if get_tree().paused==true:
		#garbage:
		#selecting
		if Input.is_action_pressed("ui_up"):
			selected_button-=1
		elif Input.is_action_pressed("ui_down"):
			selected_button+=1
		#cap 
		if selected_button>=2:
			selected_button=0
		if selected_button<=-1:
			selected_button=1	
		#cursor
		if (selected_button==0):
			cursor.position=Vector2(cursor.position.x,248)

		if (selected_button==1):
			cursor.position=Vector2(cursor.position.x,513)

		#accepting input for the buttons
		if selected_button==1 and (button_quit.pressed==true or Input.is_action_just_pressed("in_accept")):
			get_tree().paused=false
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://title_menu.tscn")
		elif selected_button==0 and (button_continue.pressed==true or Input.is_action_just_pressed("in_accept")):
			get_parent().visible=false
			get_tree().paused=false

		if selected_button==0 and button_quit.pressed==true:
			selected_button=1
		elif selected_button==1 and button_continue.pressed==true:
			selected_button=0
			
