#!/usr/bin/python
import hashlib
import sys
import urllib
import sqlite3
import smtplib
#import Medicine
from email.mime.text import MIMEText

sys.path.insert(0,'/usr/lib/python2.7/bridge/')

sqlite_file="/mnt/sda1/db/Medicine.sql3"
#connecting to the DB
conn = sqlite3.connect(sqlite_file)
c = conn.cursor()
Message1="Medication already taken for this time of the day!!!"
Message2="Medication not prescribed for this  time of the day!!!"
Message3="XXX Medication not taken for this time of the day XXX Please take and log same "
Message4="Medication Logged for this time of the day !!!!"


M="No Message"

#defining a Mailing routine to send mails
def mailSend(email,medicine,M):
	mailserver="mx.colorado.edu"
	#email="chinmay.shah@colorado.edu"
	#mailserver.login(email, "chinHS@7777")
	msg= MIMEText("Medicine: "  + medicine + "\n" + M )
	msg['Subject'] = "Medicine Reminder Confirmation"
	msg['From'] = email
	msg['To'] = email
	mailFrom = email
	mailto = email
	#`print("In Mail FUnction")
	s = smtplib.SMTP(mailserver)
	s.sendmail(mailFrom,mailto,msg.as_string())
	                        	


# defining a split function
# splits a string 
def splitCommand(command):
        base=command.split("/")
        return base


#input from arduino to python of command

#print(sys.argv[1])
if len(sys.argv) != 2:
        print ("ERROR: Must supply argument.")
        print ("USGAE: " + sys.argv[0] + " password" )
        sys.exit()
        quit()
#store the split of oommand in base ,which acts as a array 
base=splitCommand(sys.argv[1])
#print(sys.argv[1])
#base=splitCommand(Medicine.MailMessage)

#depending on the action complete the task
#print ("base3")
#print (base[3])
if base[0] == "Mail":
		#print("Insdie Mail");
		user_id=base[1]
		rfid_value=base[2]
		query =""" select email from users where userid = :user_id  """
		c.execute(query,{'user_id':user_id})		
		for row in c.fetchall():
			email = row[0]		
		#print (email)	
		query =""" select medicine_name from medicine_master where rfid_value = :rfid_value and userid = :user_id """
		c.execute(query,{'user_id':user_id,'rfid_value':rfid_value  })
		for row in c.fetchall():
			medicine_name = row[0]
			if (base[3] == "1"):
				M=Message1
			elif (base[3] == "2"):	
				M=Message2
			elif (base[3] == "3"):		
				M=Message3
			elif (base[3] == "4"):		
				M=Message4
	        message=medicine_name 
		mailSend(email,message,M)		                
		#print ("in Mail Reminder")
		
#print ("outside Mail")					
quit()