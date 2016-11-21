<?php
session_start();
//session_regenerate_id();
//Whitelist of Ips to be passed
$whitelist=array('10.201.23.52','192.168.240.1');
//Check for variable
isset($_REQUEST['i'])?$i=strip_tags($_REQUEST['i']):$i="";
isset($_REQUEST['j'])?$j=strip_tags($_REQUEST['j']):$j="";
//isset($_REQUEST['j'])?$j=$_REQUEST['k']:$j="";

//Start the bridge 
require("/usr/lib/php/bridge/bridgeclient.class.php");
$client = new bridgeclient();

isset($_REQUEST['postUser'])?$postUser=strip_tags($_REQUEST['postUser']):$postUser="";
isset($_REQUEST['postPass'])?$postPass=strip_tags($_REQUEST['postPass']):$postPass="";

isset($_REQUEST['postMedName'])?$postMedName=strip_tags($_REQUEST['postMedName']):$postMedName="";
isset($_REQUEST['postRFID'])?$postRFID=strip_tags($_REQUEST['postRFID']):$postRFID="";
isset($_REQUEST['bh'])?$bh=strip_tags($_REQUEST['bh']):$bh="";
isset($_REQUEST['lh'])?$lh=strip_tags($_REQUEST['lh']):$lh="";
isset($_REQUEST['dn'])?$dn=strip_tags($_REQUEST['dn']):$dn="";

function logout()
{   
	//session_start();
	session_destroy();
	session_unset();
	logMessage ("**INFO** Logging Out ");
	logMessage ("**INFO** Session :".$_SESSION);
	header("Location: /sd/MedicineReminder/login.php");
	logMessage ("**DEBUG** Going to login ");
	exit;
}

function logMessage($message)
{
	$message=date('Y-m-d H:i:s').$message."\n";
	error_log($message,3,"/mnt/sda1/logs/php.log");
}

function loginAttempts($db)
{	
	$stmt = $db->prepare('select count(loginid) from login where logdate >= datetime("now","localtime","-1 hour") and ip=:ip and action="fail"');
	$stmt->bindValue(':ip',$_SERVER['REMOTE_ADDR'],SQLITE3_TEXT);
	$res=$stmt->execute();
	
	logMessage ("**INFO** Session IP :".$_SERVER['REMOTE_ADDR']);	
	logMessage ("**INFO** Checking for login attempts > 5");	
	while ($row =$res->fetchArray())
	{
		$count1=$row[0];
		$count2=$row[1];
	}	
	if ($count1 == 0)
		{
			logMessage ("**INFO**  Fail logging attempts: None ");
		}
	else if ($count1 > 50 )
		{
			logMessage ("**INFO** logging attempts: ".$count1);
			logMessage ("**ERROR** User login attempts > 5");
			exit();
		}
}

function checkAuth()
{
	logMessage ("**INFO** Checking the Session Variables");	
	if(isset($_SESSION['ip']))
	{
		if ($_SESSION['ip'] != $_SERVER['REMOTE_ADDR'])
		{
			logMessage ("**ERROR** Session does not match".$_SESSION['ip']);	
			logout();
		}
	}
	else
	{
		logMessage("Session not available".$_SESSION['ip']);
		//echo "Session not there";
		logout(); 
	}
	
	if ("POST" == $_SERVER["REQUEST_METHOD"])
	{
		if (isset($_SERVER['HTTP_HOST']))
		{	
			logMessage("**ERROR** HTTP_HOST from server: ".$_SERVER["HTTP_HOST"]);
			//if (($_SERVER["HTTP_ORIGIN"]) != "https://10.201.23.52/")
			//if (($_SERVER["HTTP_ORIGIN"]) != "htpps://192.168.240.1/")
			//if (($_SERVER["HTTP_HOST"]) != "192.168.43.167")
			if (($_SERVER["HTTP_HOST"]) != "10.201.23.89")
			//if (($_SERVER["HTTP_HOST"]) != "192.168.240.1")
			{
				logMessage("**ERROR** HTTP_ORIGIN incorrect: ".$_SERVER["HTTP_HOST"]);
				logout();
			}
		}	
		else
		{
			logMessage("**ERROR** HTTP_ORIGIN incorrect: ".$_SERVER["HTTP_HOST"]);
			logout();
		}					
		logMessage ("**INFO** HTTP_ORIGIN correct: ".$_SERVER["HTTP_HOST"]);	
	}
			
	if (isset($_SESSION['HTTP_USER_AGENT']))
		{
			if($_SESSION['HTTP_USER_AGENT'] != md5($_SERVER['SERVER_ADDR'].$_SERVER['HTTP_USER_AGENT']))
			{	
				logMessage("**ERROR** User incorrect: ".$_SESSION['HTTP_USER_AGENT']);
				//echo "User incorrect";
				logout();
			}
		}
	else
		{
			logMessage ("**ERROR** User not present".$_SESSION['HTTP_USER_AGENT']);
			//echo "User not present";
			logout();
		}	
			logMessage("**INFO** User correct: ".$_SESSION['HTTP_USER_AGENT']);
			logMessage("**INFO** Non Md5 User correct: ".$_SERVER['HTTP_USER_AGENT']);
		
	if(isset($_SESSION['created']))
		{	//  for testing set to 1 
			if ( time() - $_SESSION['created'] > 1800 )
			{
				logMessage("**ERROR** Session Timed out");
				//echo "Session timeout";	
				logout();
			}
		}
		else
		{
			logMessage("**ERROR** Session Time Not Present ");
			//echo "Session not create";	
			logout();	
		}
	}

// To log the messages in Logs ....in php.log  
function loginLog($db,$postUser,$action)
{
	$stmt = $db->prepare('insert into login (ip,user,logdate,action) values (:ip,:name,datetime("now","localtime"),:action)');
	$stmt->bindValue(':name',$postUser,SQLITE3_TEXT);
	$stmt->bindValue(':ip',$_SERVER['REMOTE_ADDR'],SQLITE3_TEXT);
	$stmt->bindValue(':action',$action,SQLITE3_TEXT);
	$result=$stmt->execute();
	logMessage("**INFO** Logging into DB ");
	
}

function authenticate($db,$postUser,$postPass){ 
	
	//logMessage ("**INFO** DB from Auth ".$db);
	
	if ($postUser == Null || $postPass == Null)
	{
		//echo "\n No input .Please try again !!!";
		
		logout ();
	}
	//
	$rowCount=0;
	//
	//Checking for the fail login attempts
	loginAttempts($db);
	
	

	logMessage ("**INFO** Authenticating the Credentials");
	$stmt = $db->prepare('select userid,username,password,salt,email from users where username=:name');
	$stmt->bindValue(':name',$postUser,SQLITE3_TEXT);
	$result=$stmt->execute();
	
	while ($row =$result->fetchArray())
	{
		 $userid=$row[0];
		 $username=$row[1];
		 $password=$row[2];
		 $salt=$row[3];
		 $email=$row[4]; 
		 $epass=hash('sha256',$postPass.$salt);
		 //logMessage ("**INFO** User Id ".$userid);
		 //logMessage ("**INFO** UserName ".$username);
		 //logMessage ("**INFO** Password ".$password);
		 //logMessage ("**INFO** salt".$salt);
		 logMessage ("**INFO** epass ".$epass);
		 
		if($epass == $password)
			{	
				logMessage ("**INFO** Credentials Correct");
				session_regenerate_id();
				$_SESSION['ip']=$_SERVER['REMOTE_ADDR'];
				logMessage("**INFO** : Remote Address ".$_SERVER['REMOTE_ADDR']);
				//Checking for User Agent
				$_SESSION['HTTP_USER_AGENT']=md5($_SERVER['SERVER_ADDR'].$_SERVER['HTTP_USER_AGENT']);
				//
				logMessage("**INFO** : Http user agent ".$_SESSION['HTTP_USER_AGENT']);
				$_SESSION['created']=time();
				//
				logMessage("**INFO** : Created time ".$_SESSION['created']);
				$_SESSION['userid']=$userid;
				//
				logMessage("**INFO** : User Id  ".$_SESSION['userid']);
				$_SESSION['email']=$email;
				//
				logMessage("**INFO** : email id ".$_SESSION['email']);
				
				$_SESSION['authenticated']="yes";
				logMessage("**INFO** : authenticated ".$_SESSION['authenticated']);
				//$_SESSION['HTTP_ORIGIN']="yes";
				logMessage("**INFO** : orgin ".$_SERVER['HTTP_ORIGIN']);
				logMessage("**INFO** : host ".$_SERVER['HTTP_HOST']);
				$rowCount++;
				loginLog($db,$postUser,"pass");
				
				
				logMessage ("**INFO** Session :".$_SESSION);
			}	
		else
		{		
				loginLog($db,$postUser,"fail");
				logMessage("**ERROR** : Failed Login from ".$epass);
				logMessage("**ERROR** : Failed Login from ".$password);
				logMessage("**INFO** Credentials Incorrect");
				logMessage("**ERROR** : Failed Login from ".$_SERVER['REMOTE_ADDR']);
				logout();
		}
	}
	if ($rowCount == 0)
		{	
			loginLog($db,$postUser,"fail");
			logMessage("**ERROR** : Row Count ".$rowCount);
			logMessage("**ERROR** : Failed Login from ".$_SERVER['REMOTE_ADDR']);
			logout();
		}		
	
	
}

function checkGroup($db,$postUser){
			logMessage ("**INFO** Checking for Group for User :".$postUser);
			$stmt = $db->prepare('select usergroup,users.userid from users,user_group where username=:name and users.userid=user_group.userid');
			$stmt->bindValue(':name', $postUser, SQLITE3_TEXT);
			$group=$stmt->execute();
			//logMessage ("**INFO** Group for User :".$group);
			while ($row1 =$group->fetchArray()){
					$group1=$row1[0];
					$group2=$row1[1];
					//$group3=$row1[2];
			}
			logMessage ("**INFO** Group for User :".$group1);
			logMessage ("**INFO** userid for User :".$group2);
			//logMessage ("**INFO** userid for User :".$group3);			
			$_SESSION['group']=$group1;
			
	}

function WebLed(){
		echo 
		"<html>
		<head><title> PHP First Form </title></head>
		<body>
		<form method =post action=WebLed.php>
		<a href =WebLed.php?i=1>LEDON</a><br>
		<a href =WebLed.php?i=2>LEDOFF</a><br>
		<a href =WebLed.php?i=3>LEDSTATUS</a><br>
		";
		if ($i == "1")
			{
			$client->put('lightCommand','ledon');
			echo "<font color=\"green\">****Turning Led ON**** </font>";
			}
		else if ($i == "2")
			{
			$client->put('lightCommand','ledoff');
			echo "<font color=\"red\">****Turning Led OFF**** </font>";
			}
		else if ($i == "3")
			{
			//single quotes	
			$client->put('lightCommand','ledstatus');
			//Double quotes required
			$j=$client->get("arduinoCommand");
				if ($j == "1")
				{
					echo "<font color=\"blue\">****Led Status: ON**** </font>";
				}
				else
				{
					echo "<font color=\"blue\">****Led Status: OFF**** </font>";
				}	
			
			}	
		echo
		" 
		</form>
		</body>
		</html>
		";	
}
function AddMedicines($db){
	echo "

	<head><title> Add Medicines </title></head>
	<body>
	<br><center>
	<form method =post action=index.php>
	<H3> Add New Medicines </H3>
	<br>Medicine's Name	:
	<input type=\"text\" name=\"postMedName\"/>
	<br>CardValue  	    :  
	<input type=\"text\" name=\"postRFID\"/>
	<br><input type=\"Submit\" name=\"SubmitRFID\" value=\"SubmitMedicine\"/>
	  ";
	 AddMedicinesValue($db);  

	
}


function AddMedicinesValue($db,$postMedName,$postRFID,$bh,$lh,$dn)  {
	logMessage("**INFO** auth".$postMedName.$postRFID);
	if ($postMedName == Null || $postRFID == Null){
		logMessage("**ERROR**  Fields are empty ");
		//logout ();
	}
	else{
		$stmt = $db->prepare('insert into medicine_master (userid,medicine_name,rfid_value,bh,lh,dn,dateentry) values (:userid,:medname,:rfid,:bh,:lh,:dn,datetime("now","localtime"))');
	//	logMessage("**INFO**  User value ".$_SESSION['userid']);
		$stmt->bindValue(':userid',$_SESSION['userid'],SQLITE3_TEXT);
		$stmt->bindValue(':medname',$postMedName,SQLITE3_TEXT);
		$stmt->bindValue(':rfid',$postRFID,SQLITE3_TEXT);
		$stmt->bindValue(':bh',$bh,SQLITE3_INTEGER);
		$stmt->bindValue(':lh',$lh,SQLITE3_INTEGER);
		$stmt->bindValue(':dn',$dn,SQLITE3_INTEGER);
		$result=$stmt->execute();
		logMessage("**INFO** Adding Medcines in DB ");
	}	
}
	
?>