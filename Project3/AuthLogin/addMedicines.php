<?php
session_start();
//session_regenerate_id();
include_once('/mnt/sda1/phplib/auth-lib.php');
$db=new SQLite3("/mnt/sda1/db/Medicine.sql3");
include_once('header.php');
logMessage("**INFO** Index".$postMedName.$postRFID);
AddMedicinesValue($db,$postMedName,$postRFID,1,1,1);  
logMessage("**INFO** In Addition page");						
	
include_once('footer.php');	
?>