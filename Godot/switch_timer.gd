extends Node2D

onready var get_estimate_time_secs=0
onready var get_estimate_time_msecs=0
onready var frames = 0

func _process(_delta):
    frames+=1
    get_estimate_time_msecs+=17
    if frames==60:
        get_estimate_time_secs+=1
        frames=0
