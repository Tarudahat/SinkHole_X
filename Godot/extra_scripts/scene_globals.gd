extends Node2D

onready var level =-1
onready var difficulty =-1
onready var total_score=0
onready var added_score=0
onready var player_score=0
onready var hi_scores=[[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]
                    #hi_scores[x][y]
                    #  x = what level | y = what difficulty of that level
