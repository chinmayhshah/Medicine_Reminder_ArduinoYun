<?php
$randomNum=rand();
$salt=hash(sha256,$randomNum);
$password="arduino";
$epass=hash(sha256,$password.$salt);

echo "Salt: $salt <br>
echo Pass: $password <br>
echo EPass: $epass <br>

insert statement: <br>
insert into users (username,password,salt,email) values ('chinmay','$epass','$salt','chinmayhshah1989@gmail.com');
";
?>