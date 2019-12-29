import smtplib
 
#username and pass
username = '123456789hcm'
password = '123456abc'
 
#creat email
msg = 'This is a test'
  
server = smtplib.SMTP('smtp.gmail.com:587')
server.starttls() #Connect TLS SMTP
server.login(username, password) # login user, pass
 
fromaddr = '123456789hcm@gmail.com' # address sender
toaddr = 'xxx.@gmail.com' # addressee
 
server.sendmail(fromaddr, toaddr, msg) # send mail
 
server.close() # end