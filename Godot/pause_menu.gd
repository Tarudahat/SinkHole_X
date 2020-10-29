extends TextureRect

onready var button_continue = get_node("continue_button")
onready var button_quit = get_node("quit_button")
onready var selected_button = 0
onready var continue_ = false
onready var quit_ = false

func _process(_delta):

	#hmm maybe I should do this with UI focus...

	if Input.is_action_pressed("ui_up"):
		selected_button-=1
	elif Input.is_action_pressed("ui_down"):
		selected_button+=1

	if selected_button>=2:
		selected_button=0
	if selected_button<=-1:
		selected_button=1	

	if (selected_button==0):
		if Input.is_action_just_pressed("in_accept"):
			continue_=true
	if (selected_button==1):
		if Input.is_action_just_pressed("in_accept"):
			quit_=true

	if button_quit.pressed==true or quit_==true:
		quit_=false
		get_tree().paused=false
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://title_menu.tscn")
	elif button_continue.pressed==true or Input.is_action_just_pressed("in_paused") or continue_==true:
		continue_=false
		get_parent().visible=false
		get_tree().paused=false
		
