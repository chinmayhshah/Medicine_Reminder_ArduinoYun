#!/usr/bin/python
#import hashlib
import sys
#For Connection to DB
import sqlite3

import os


#Using datetime module
from datetime import datetime

#localtime = time.localtime(time.time())
# for formatted time 
#localtime = time.asctime(time.localtime(time.time()) )
now = datetime.now()

sqlite_file="/mnt/sda1/db/Medicine.sql3"



table_name2="medicine_master"
column_name2="userid"
column_name3="rfid_value"

#connecting to the DB
conn = sqlite3.connect(sqlite_file)
c = conn.cursor()

#Initializing medtime
medtime=now.replace(hour=0, minute=0, second=0, microsecond=0)

# defining a split function
# splits a string 
def splitCommand(command):
        base=command.split("/")
        return base

		
def readFile(readFile):
        fileRead = open(readFile,'r')
        ReadString = fileRead.read()
        fileRead.close()
        return ReadString

# Writing text to file
# writeFile   -file written to 
# writeString - text to be written
def writeFile(writefile,writeString):
                fileWrite = open(writefile,'wd')
                fileWrite.write(writeString)
                fileWrite.close()
                
# To find the hour of the day 
def medicationtime (bhhour,lhhour,dnhour):
	bhhour=now.replace(hour=bhhour, minute=0, second=0, microsecond=0)
	lhhour=now.replace(hour=lhhour, minute=0, second=0, microsecond=0)
	dnhour=now.replace(hour=dnhour, minute=0, second=0, microsecond=0)

	if now < bhhour:
		#print("Brekfast Hours")
		medtime="bh"
	elif (now > bhhour and now < lhhour):
		#print("Lunch Hours")
		medtime="lh"
	elif now > dnhour:
		#print("Dinner Hours")
		medtime="dn"		
		




#input from arduino to python of command
if len(sys.argv) != 2:
        print ("ERROR: Must supply argument.")
        print ("USGAE: " + sys.argv[0] + " password" )
        sys.exit()
        quit()
#store the split of oommand in base ,which acts as a array 
base=splitCommand(sys.argv[1])

#print "now :",now

#now = datetime.now()


 
	
#depending on the action complete the task
if base[0] == "CheckRFID":
	my_value=base[2]
	user_id=base[1]
		
	query =""" select * from user_data where rfid_value = :my_value and userid = :user_id and date_taken= date('now','localtime') """
	c.execute(query,{'my_value':my_value,'user_id':user_id})
	id_exists = c.fetchone()
	if id_exists:
		medicationtime(2,16,20)		
		#print(medtime)
		query2 =""" select bh,lh,dn from user_data where rfid_value = :my_value and userid = :user_id"""
		c.execute(query2,{'my_value':my_value,'user_id':user_id})
 		for row in c.fetchall():
 			bh,lh,dn = row
 			#print ('single columns1:',bh,lh,dn)
 		if (medtime == 'bh'):
 			print (bh)
 		elif (medtime == 'lh'):
 			print (lh)
 		else:	
 			print (dn)	
 		if (bh or lh or dn):
 			M="1"
		else:	
			M="2"
 			
	else:
		print('0')
		M="3"
		
elif base[0] == "TakeMed":
	my_value=base[2]
	user_id=base[1]
	bh=0
	lh=0
	dn=0
	
	medicationtime(2,16,20)		
 	if (medtime == 'bh'):
 		bh=1				
 	elif (medtime == 'lh'):
 		lh=1
 	else:	
 		dn=1	
	
	query =""" select * from user_data where rfid_value = :my_value and userid = :user_id and date_taken= date('now','localtime') """
	c.execute(query,{'my_value':my_value,'user_id':user_id})
	
	id_exists = c.fetchone()
	if id_exists:
			
	 	if (medtime == 'bh'):
 			query="""update user_data set bh=:bh  where  rfid_value = :my_value and userid = :user_id and date_taken=date('now','localtime') """				
 		elif (medtime == 'lh'):
			query="""update user_data set lh=:lh  where  rfid_value = :my_value and userid = :user_id and date_taken= date('now','localtime') """
		else:
			query="""update user_data set dn=:dn  where  rfid_value = :my_value and userid = :user_id and date_taken= date('now','localtime') """
		c.execute(query,{'my_value':my_value,'user_id':user_id,'lh':lh,'bh':bh,'dn':dn})	
		M="4"								 			
	
	else:			
		query="""insert into user_data (userid,rfid_value,bh,lh,dn,logdate,date_taken)  values (:user_id,:my_value,:bh,:lh,:dn,datetime("now","localtime"),date("now","localtime"))"""
		c.execute(query,{'my_value':my_value,'user_id':user_id,'lh':lh,'bh':bh,'dn':dn})	
	# Mailing the confirmation of the Medicine taken to the user	
	M="4"
MailMessage= "Mail" + "/" + base[1] + "/"  + base[2] + "/" + M 
MailCommand = "python /mnt/sda1/scripts/MedicineReminderMail.py " + MailMessage 
#print (MailCommand) 
os.system (MailCommand)
#Closing the Connection with DataBase File
#print (MailCommand)
conn.commit()
conn.close()						
quit()