extends CheckButton

var prev_toggle=false
var on_off_list=[false]

func _ready():
	for children in self.get_children():
		on_off_list.append(false)

func _input(_event):
	
	if len(self.get_children())>=1:
		var i=0
		for children in self.get_children():
			if prev_toggle!=self.pressed:
				children.pressed=self.pressed
		for children in self.get_children():		
			if children.pressed==true and self.pressed==false:
				self.pressed=true
			on_off_list[i+1]=children.pressed
			i+=1
		on_off_list[0]=self.pressed
			
			
	prev_toggle=self.pressed
		
