<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title>Wake on LAN</title>
		<link rel="stylesheet" href="style.css"/>
		<script type="text/javascript" src="script.js" defer></script>
	</head>
	<body>
		<form>
			<label for="type">Broadcast Address: 
				<input type="text" name="bcastIp" value="192.168.0.255"/>
			</label>
			<label for="type">MAC Address: 
				<input type="text" name="mac" value="00:1E:EC:03:F5:0A"/>
			</label>
			<label for="type">PC IP Address: 
				<input type="text" name="ip" value="192.168.0.2"/>
			</label>
			<input type="submit" value="Wake up PC">
		</form>
		<?php
			define('EOL', '<br/>');
			if (isset($_GET['mac']) && isset($_GET['bcastIp']) && isset($_GET['ip'])) {
				$mac = trim($_GET['mac']);
				$bcastIp = trim($_GET['bcastIp']);
				$ip = trim($_GET['ip']);
				exec("./wol -r $bcastIp $mac", $output, $result);
				$status = $result ? 'failed' : 'success';
				echo '<input type="hidden" id="pc-ip" value="' . $ip . '"/>';
				echo '<input type="hidden" id="result" value="' . $status . '"/>';
				echo $result ?  '<p class="red">' : '<p class="green">';
				array_map(function($x) {
					echo str_replace('\n', EOL, $x) . EOL;
				}, $output);
				echo '</p>';
			}
			else echo '<input type="hidden" id="result" value="failed"/>';
		?>
		<div id="loader"></div>
		<p id="pc-status"></p>
	</body>
</html>

