extends KinematicBody2D

onready var score=0
onready var player_x=0
onready var player_y=0
onready var player_state=0
onready var bridges=0
onready var speed=1
onready var sprite=self.get_child(0)
var player_collision
var invincible = false

func _process(_delta):
	player_x=0
	player_y=0
	if Input.is_action_pressed("in_right"):
		player_x+=1
		score+=10*speed
		self.rotation_degrees=90
	elif Input.is_action_pressed("in_left"):
		player_x-=1
		score+=10*speed
		self.rotation_degrees=270
	if Input.is_action_pressed("in_up"):
		player_y-=1
		score+=10*speed
		self.rotation_degrees=0
	elif Input.is_action_pressed("in_down"):
		player_y+=1
		score+=10*speed
		self.rotation_degrees=180

	if self.position.x>20*64:
		self.position.x=0
	if self.position.x<0:
		self.position.x=20*64
	if self.position.y>12*64:
		self.position.y=0
	if self.position.y<0:
		self.position.y=12*64

	player_collision=move_and_collide(Vector2(player_x,player_y).normalized()*Vector2(3*speed,3*speed))
