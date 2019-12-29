import msvcrt

strResult = ""
while True:
    pressedKey = msvcrt.getch()
    if pressedKey == ',':    
       print strResult
       strResult = ""
    else:
       strResult += str(pressedKey)