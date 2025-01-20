import mysql.connector

myCon = mysql.connector.connect(
    host = 'localhost',
    user = 'root',
    passwd = '921729#Spks',
    database = 'Authorised_Vehicles'
)

cursor = myCon.cursor()

#cursor.execute('create table Owner_Info(Owner_iD varchar(10), Owner_Name varchar(15), Address varchar(20), Age int)')

cursor.execute('show tables;')

result = cursor.fetchall()

for i in result:
    print(i)

myCon.commit()

cursor.execute('select * from vehicleList;')

VehicleList = cursor.fetchall()

for i in VehicleList:
    print(i)
