extends TextEdit

export var MAX_CHAR=0

func _process(delta):
	self.text=self.text.replace("\n","")
	self.text=self.text.replace(" ","-")
	if len(self.text)>=MAX_CHAR:
		self.text=self.text.substr(0,MAX_CHAR)
