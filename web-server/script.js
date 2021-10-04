var res = document.getElementById ('result').value;
var div = document.getElementById ('pc-status');
var ipdiv = document.getElementById ('pc-ip');
var loader = document.getElementById ('loader');
var ip = (ipdiv) ? ipdiv.value : null;
var interval;
var awaken = false;
console.log ('ip: ' + ip);

function ping (ip)
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function()
	{
		if (xhr.readyState == 4 && xhr.status == 200)
		{
			var res = parseInt (xhr.responseText);
			if (!isNaN (res)) switch (res)
			{
				case -2:
					console.log ("Request failed");
				break;
				case -1:
					console.log ("Invalid IP address");
				break;
				case 1:
					loader.style.display = 'none';
					awaken = true;
					div.innerHTML = "IT'S</br>ALIVE...";
					console.log ('pc[' + ip + '] is successfuly awaken');
				break;
				case 0:
					console.log ('Still dead...');
				break;
			}
		}
	};

	if (!awaken)
	{
		xhr.open ('GET', 'ping.php?ip=' + ip, true);
		xhr.send();
	}
	else clearInterval (interval);
}

if (res == 'success' && ip)
{
	loader.style.display = 'block';
	inerval = setInterval (ping, 3000, ip);
}
