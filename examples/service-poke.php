<?php
	$error = service_poke('/tmp/poke.sock', 'foo');
	if($error !== FALSE)
		print $error . "\n";
?>
