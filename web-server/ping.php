<?php
if (isset ($_GET['ip']))
{
	$ip = trim ($_GET['ip']);
	if (preg_match ('/^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/', $ip))
	{
		exec ("ping -c 1 -w 1 $ip", $out, $result);
		echo $result ? 0 : 1;
	}
	else echo -1;
}
else echo -2;
