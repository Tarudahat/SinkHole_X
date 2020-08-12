extends Area2D

onready var collided_with_player = false
onready var enemy_direction = 1

func _on_enemy_body_entered(_body):
	collided_with_player = true
