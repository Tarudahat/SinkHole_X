extends Node2D

onready var level =-1
onready var difficulty =-1

onready var total_score=0
onready var added_score=0
onready var player_score=0
onready var hi_scores=[[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]
                    #hi_scores[x][y]
                    #  x = what level | y = what difficulty of that level

onready var levels=["Plains","Highway","Snow field","Volcano","Phantom field","Random","Custom"]
onready var prices=[0,150000,300000,450000,600000,150000 ,1000000]
onready var unlocked=[true,false,false,false,false,false,false]
onready var new_lvl_id=-1
