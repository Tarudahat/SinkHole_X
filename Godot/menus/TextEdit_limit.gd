extends TextEdit

export var MAX_CHAR=0
var backup_text=""

func _process(delta):

	if self.text.is_valid_integer():
		self.text=self.text.replace("\n","")
		if len(self.text)>=MAX_CHAR:
			self.text=self.text.substr(0,MAX_CHAR)
		backup_text=self.text
	elif self.text=="":
		backup_text=""
	else:
		self.text=backup_text

