import MySQLdb

# mo ket noi toi Database
db = MySQLdb.connect("localhost","sa","123456","TESTDB" )

# chuan bi mot doi tuong cursor boi su dung phuong thuc cursor()
cursor = db.cursor()

# Xoa bang neu no da ton tai boi su dung phuong thuc execute().
cursor.execute("DROP TABLE IF EXISTS SINHVIEN")

# Tao mot bang
sql = """CREATE TABLE SINHVIEN (
         HO  CHAR(20) NOT NULL,
         TEN  CHAR(20),
         TUOI INT,  
         GIOITINH CHAR(1),
         HOCPHI FLOAT )"""

cursor.execute(sql)

# ngat ket noi voi server
db.close()