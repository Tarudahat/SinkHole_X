extends Area2D

onready var collided_with_player = false
onready var enemy_direction = 1
onready var enemy_type = 0 #0=car 1=snowman 2=snow_ball

onready var target = Vector2(0,0)
onready var can_spawn=true

onready var linked_node=Node


func _on_enemy_body_entered(_body):
	collided_with_player = true
