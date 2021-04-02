extends Node2D

var using=false

#which base should be used (level)
var level_base=0

#1st -> using items?, rest is used items 
var using_items=[false,false,false,false,false,false,false]

#1st -> using enemies?, rest is used enemies
var using_enemies=[false,false,false,false,false]

var using_sound_fx=false

#song's id-s are determent by their arangment in the editor
var using_music=0
