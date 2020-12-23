extends KinematicBody2D

onready var score=0
onready var player_x=0
onready var player_y=0
onready var player_state=0
onready var bridges=0
onready var speed=1
onready var sprite=self.get_child(0)

func _process(_delta):
	if Input.is_action_pressed("in_right"):
		player_x+=3*speed
		score+=10*speed
		self.rotation_degrees=90
	elif Input.is_action_pressed("in_left"):
		player_x-=3*speed
		score+=10*speed
		self.rotation_degrees=270
	if Input.is_action_pressed("in_up"):
		player_y-=3*speed
		score+=10*speed
		self.rotation_degrees=0
	elif Input.is_action_pressed("in_down"):
		player_y+=3*speed
		score+=10*speed
		self.rotation_degrees=180

	if player_x>20*64:
		player_x=0
	if player_x<0:
		player_x=20*64
	if player_y>12*64:
		player_y=0
	if player_y<0:
		player_y=12*64

	self.position=Vector2(player_x,player_y)
