import string
import random
import re
# strValidate = string.letters + string.digits
# password = "".join(random.choice(strValidate) for _ in range(random.randint(0,8)))
# password = "".join(random.choice(strValidate) for _ in range(8))

def generatedPass():
    strValidate = string.letters + string.digits
    return "".join(random.choice(strValidate) for _ in range(8))

def checkPass(password):
    strValidate = string.letters + string.digits
    return all(c in strValidate for c in password) and len(password) == 8

passw = generatedPass()
print passw
if not checkPass(passw):
    print "Invalid password"