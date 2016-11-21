<?php
session_start();
//session_regenerate_id();
include_once('header.php');
echo "

<head><title> Login Form </title></head>
<body>
<br><center>
<form method =post action=index.php>
<H3> Login </H3>
	<br>Username:
	<input type=\"text\" name=\"postUser\"/>
	<br> Password  :  
	<input type=\"password\" name=\"postPass\"/>
    <br><input type=\"Submit\" name=\"SubmitPass\" value=\"Login\"/>
  

";
include_once('footer.php');	
?>