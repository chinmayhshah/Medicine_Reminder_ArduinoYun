<?php
	session_start();
	//session_regenerate_id();
	
	include_once('/mnt/sda1/phplib/auth-lib.php');
	$db=new SQLite3("/mnt/sda1/db/Medicine.sql3");
	//connect($db);
	//echo "Inside index ";	
	//Checking for valid session_cache_expire
	
	
	if(isset($_POST['SubmitRFID'])){
		
		AddMedicinesValue($db,$postMedName,$postRFID,$bh,$lh,$dn);
	}
	
	logMessage ("**INFO** Index page for Medicine reminder");	
	
		if(!isset($_SESSION['authenticated']))
			{
				authenticate($db,$postUser,$postPass);	
				checkGroup($db,$postUser);	
				//checkGroup();
			}
		#Checking for session variables
		checkAuth();
		
		include_once('header.php');
		
		
		
		if ($_SESSION['group'] == 'U'){
				logMessage("**INFO** Routing to Patient Page");
				echo 
					"
					<head><title> PHP First Form </title></head>
					<body>
					<form method =post action=index.php>
					<a href =index.php?i=1>ADD NEW MEDICINE</a><br>
					<a href =index.php?i=2>CHECK MEDICINE</a><br>
					<a href =index.php?i=3>LOG MEDICATION </a><br>
					<a href =index.php?i=4>MONTHLY REPORT</a><br>
					";
					//logMessage("**INFO** Value of i".$_REQUEST['i']);
					if ($i == "1"){
							$client->put('lightCommand','Capture');
							echo "<font color=\"blue\">****In Addition  of    Medicines **** </font>";
							//AddMedicines();
							//$i=5;
							$j=$client->get("arduinoCommand");
								if ($j != ""){
									logMessage ("**INFO** Value of RFID ".$j);	
									echo "<font color=\"blue\">****RFID Value : **** </font>";
									echo "<font color=\"green\">****$j **** </font>";
								}
								else{
									logMessage ("**INFO** No RFID Value captured ");
									echo "<font color=\"red\">****No RFID Value : **** </font>";
								}	
							
							
							echo "
							
								<br><center>
								<form method =post action=index.php>
								<H3> Add More New Medicines </H3>
								<br>Medicine's Name	:
								<input type=\"text\" name=\"postMedName\"/>
								<br>Medicine's Card :  
								<input type=\"text\" name=\"postRFID\" value=\"$j\" readonly/>
								<br><input type=\"checkbox\" name=\"bh\" value=\"1\" /> Breakfast
								<br><input type=\"checkbox\" name=\"lh\" value=\"1\" /> Lunch
								<br><input type=\"checkbox\" name=\"dn\" value=\"1\" /> Dinner
								<br><input type=\"Submit\" name=\"SubmitRFID\" value=\"Add Medicine\"/>
								</form>
							"; 
							
					}
					else if ($i == "2"){
							$userid=$_SESSION['userid'];
							$ArduinoCommand = "CheckRFID"."/".$userid."/";
							$client->put('lightCommand',$ArduinoCommand);
							//$client->put('userid',$ArduinoCommand);
							//echo "<font color=\"red\">****Turning Led OFF**** </font>";
							echo "<br><font color=\"blue\">****To Check Medication Place medicine near Reader **** </font>";
							logMessage("**INFO** In Check of Medication");
					}
					else if ($i == "3"){
							$userid=$_SESSION['userid'];
							$ArduinoCommand = "TakeMed"."/".$userid."/";
							$client->put('lightCommand',$ArduinoCommand);
							//$client->put('userid',$ArduinoCommand);
							//echo "<font color=\"red\">****Turning Led OFF**** </font>";
							echo "<br><font color=\"blue\">****For the medication taken **** </font>";
							echo "<br><font color=\"blue\">****Place the Medicine near Reader **** </font>";
							logMessage("**INFO** In Medication taken ");
						
					}
					else if ($i == '4'){
						echo "<font color=\"blue\">****In Monthly REPORT**** </font>";
						logMessage("**INFO** In Monthly Reports");
						
					}
					else if ($i == "5")
							{	
							logMessage("**INFO** Index".$postMedName.$postRFID);
							AddMedicinesValue($db,$postMedName,$postRFID,1,1,1);  
							logMessage("**INFO** In Addition  of Medicines");	
							}	
		
				//PatientPage();
		}
		else	if ($_SESSION['group'] == 'A'){
				logMessage("**INFO** Routing to Admin Page");
				//AdminPage();
				echo 
					"
					<head><title> PHP First Form </title></head>
					<body>
					<form method =post action=index.php>
					<a href =index.php?i=1>DAILY REPORT</a><br>
					<a href =index.php?i=2>MONTHLY REPORT</a><br>
					
					";
					if ($i == "1"){
							$client->put('lightCommand','ledon');
							echo "<font color=\"green\">**** Daily Reports**** </font>";
					}
					else if ($i == "2"){
							$client->put('lightCommand','ledoff');
							echo "<font color=\"red\">****Monthly Reports**** </font>";
					}
				
		
				
			
		}
			
	include_once('footer.php');				
?> 