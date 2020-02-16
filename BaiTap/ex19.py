# import sqlite3 as lite
# import sys
# import os
# con = None
 
# try:
#     path = os.path.dirname(__file__) + "\\test.db"
#     con = lite.connect(path)
     
#     cur = con.cursor()    
#     cur.execute('SELECT SQLITE_VERSION()')
     
#     data = cur.fetchone()
     
#     print ("SQLite version: %s" % data)         
     
# except lite.Error as e:
     
#     print ("Error %s:" % e.args[0])
#     sys.exit(1)
     
# finally:
     
#     if con:
#         con.close()

import MySQLdb

db = None
try:
    db = MySQLdb.connect(host = 'localhost', user ='root', passwd = 'root', db = 'mysql')
    
except MySQLdb.Error, e:
    print "Error %d: %s" % (e.args[0],e.args[1])
    sys.exit(1)

if db:
    cur = db.cursor()
    cur.execute("SELECT VERSION()")
    ver = cur.fetchone()