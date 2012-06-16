function warning_banner()
{
	if(window.ActiveXObject)
	{
		document.write("<div style='text-align: center;'><span style='color: red; font-size: x-small'>This website is optimized for Mozilla Firefox 3. Optimization for Microsoft Internet Explorer and other browsers is in progress...</span></div>");
	}
}

function clientSideInclude(url, from, to)
{
	var req = false;
	var error = false;

	if(window.XMLHttpRequest)
	{
		// For Safari, Firefox, and other non-MS browsers
		try
		{
			req = new XMLHttpRequest();
		}
		catch(e)
		{
			error = true;
		}
	}
	else if (window.ActiveXObject)
	{
		// For Internet Explorer on Windows
		try
		{
			req = new ActiveXObject("Msxml2.XMLHTTP");
		}
		catch(e)
		{
			try
			{
				req = new ActiveXObject("Microsoft.XMLHTTP");
			}
			catch(e)
			{
				error = true;
			}
		}
	}

	if(!error && req)
	{
		// Synchronous request, wait till we have it all
		try
		{
			req.open('GET', url, false);
			req.send(null);
			i=req.responseText.indexOf(from);
			j=req.responseText.indexOf(to, i);
			if(i==-1 || j==-1)
			{
				document.write(
					"<br><i><b>Sorry, some part of <a href=\"" + url + "\">this page</a> " +
					"should have been printed in this place but could not be " +
					"found. Please, report this bug to the webmaster.</b></i><br>");

				document.write("<br>" + i);
				document.write("<br>" + j);
			}
			else
			{
				document.write(req.responseText.substring(i, j+to.length));
			}
		}
		catch(e)
		{
			error = true;
		}
	}

	if(error)
	{
		document.write(
			"<br><i><b>Sorry, your browser does not support " +
			"XMLHTTPRequest objects. This page requires " +
			"Internet Explorer 5 or better for Windows, " +
			"or Firefox for any system, or Safari. Other " +
			"compatible browsers may also exist.</b></i><br>");
	}
}

function get_element_by_id(id)
{
	if (typeof id != 'string')
	{
		return id;
	}

	if (typeof document.getElementById != 'undefined')
	{
		return document.getElementById(id);
	}
	else if (typeof document.all != 'undefined')
	{
		return document.all[id];
	}
	else if (typeof document.layers != 'undefined')
	{
		return document.layers[id];
	}
	else
	{
	alert('failed');
		return null;
	}
}

function remove_div(pass)
{
	var divs = document.getElementsByTagName('div');
	for(i=0; i<divs.length; i++)
	{
		if(divs[i].id == pass)
		{ //if they are 'see' divs
			if (document.getElementById) // DOM3 = IE5, NS6
			{
				divs[i].style.display = "none";
			}
			else if (document.layers) // Netscape 4
			{
				document.layers[divs[i]].display = 'none';
			}
			else // IE 4
			{
				document.all.hideshow.divs[i].display = 'none';
			}
		}
	}
}

function remove_all_divs()
{
	remove_div('home');
	remove_div('about');
	remove_div('features');
	remove_div('jobs');
	remove_div('download');
	remove_div('support');
	remove_div('bci');
	remove_div('medias');
	remove_div('publications');
	remove_div('contributions');
}

function hide_div(pass)
{
	var divs = document.getElementsByTagName('div');
	for(i=0; i<divs.length; i++)
	{
		if(divs[i].id == pass)
		{ //if they are 'see' divs
			if (document.getElementById) // DOM3 = IE5, NS6
			{
				divs[i].style.visibility = "hidden";
			}
			else if (document.layers) // Netscape 4
			{
				document.layers[divs[i]].display = 'hidden';
			}
			else // IE 4
			{
				document.all.hideshow.divs[i].visibility = 'hidden';
			}
		}
	}
}

function hide_all_divs()
{
	hide_div('home');
	hide_div('about');
	hide_div('features');
	hide_div('jobs');
	hide_div('download');
	hide_div('support');
	hide_div('bci');
	hide_div('medias');
	hide_div('publications');
	hide_div('contributions');
}

function show_div(pass)
{
	var divs = document.getElementsByTagName('div');
	for(i=0; i<divs.length; i++)
	{
		if(divs[i].id == pass)
		{ //if they are 'see' divs
			if (document.getElementById) // DOM3 = IE5, NS6
			{
				divs[i].style.visibility = "visible";
				divs[i].style.display = "block";
			}
			else if (document.layers) // Netscape 4
			{
				document.layers[divs[i]].display = 'visible';
			}
			else // IE 4
			{
				document.all.hideshow.divs[i].visibility = 'visible';
				document.all.hideshow.divs[i].display = 'block';
			}
		}
	}
}

function show_all_divs()
{
	show_div('home');
	show_div('about');
	show_div('features');
	show_div('jobs');
	show_div('download');
	show_div('support');
	show_div('bci');
	show_div('medias');
	show_div('publications');
	show_div('contributions');
}

function select_div(pass)
{
	remove_all_divs();
	show_div(pass);
}

function available_soon(show_it)
{
	if(show_it)
		return "<br><span style='font-weight: bold; color: red;'>This will be available soon, stay tuned.</span>";
	else
		return "";
}

function screenshot(url, comment)
{
	document.write("<div class='media'><p><a href='"+url+"' alt='"+url+"'><img src='"+url+"' class='screenshot'></a>"+comment+available_soon(url=="#FIXME")+"</p></div>");
}

function video(url, snapshot, comment)
{
	document.write("<div class='media'><p><a href='"+url+"' alt='"+url+"'><img src='"+snapshot+"' class='screenshot'></a>"+comment+available_soon(url=="#FIXME")+"</p></div>");
}

function archive(name, baseurl, releasedate, comment, screenshot)
{
	document.write("<div class='archive'><p>");
	if(screenshot!=undefined)
	{
		document.write("<img src='"+screenshot+"' class='screenshot'>");
	}
	document.write("<a href='"+baseurl+".zip' class='archive'>"+name+" sources</a>");
	if(releasedate!=undefined)
	{
		document.write(" ("+releasedate+")");
	}
	document.write("</p><p>");
	document.write(comment+available_soon(baseurl=="#FIXME"));
	document.write("</p></div>");
}

function virtualmachine(name, baseurl, releasedate, comment, screenshot)
{
	document.write("<div class='virtualmachine'><p>");
	if(screenshot!=undefined)
	{
		document.write("<img src='"+screenshot+"' class='screenshot'>");
	}
	document.write("<a href='"+baseurl+".vdi.7z' class='virtualmachine'>"+name+" virtual hard disk</a>");
	if(releasedate!=undefined)
	{
		document.write(" ("+releasedate+")");
	}
	document.write("</p><p>");
	document.write("<a href='"+baseurl+".xml' class='virtualmachine'>"+name+" virtual machine</a>");
	document.write("</p><p>");
	document.write(comment+available_soon(baseurl=="#FIXME"));
	document.write("</p></div>");
}

function precompiled(name, baseurl, releasedate, comment, screenshot, height)
{
	document.write("<div class='installer'"+(height==undefined?"":" style='height: "+height+"px'")+"><p>");
	if(screenshot!=undefined)
	{
		document.write("<img src='"+screenshot+"' class='screenshot'>");
	}
	document.write("<a href='"+baseurl+".exe' class='installer'>"+name+" Installer</a>");
	if(releasedate!=undefined)
	{
		document.write(" ("+releasedate+")");
	}
	document.write("</p><p>");
	document.write(comment+available_soon(baseurl=="#FIXME"));
	document.write("</p></div>");
}

function checkRegisterForm()
{
	g_name=document.register.name.value;
	g_email=document.register.email.value;
	g_country=document.register.country.value;
	g_entity=document.register.entity.value;
	g_entityname=document.register.entityname.value;
	g_otherinfo=document.register.otherinfo.value;

	if(g_name=='')
	{
		alert("You did not enter your name."); 
		return false;
	}

	if(g_email.indexOf('@')==-1 || g_email.indexOf('.')==-1 || g_email.length <= 7)
	{
		alert("The e-mail adress you have entered seems to be invalid.\nPlease check it again."); 
		return false;
	}

	if(g_country=='')
	{
		alert("You did not enter your country."); 
		return false;
	}

	if(g_entity=='')
	{
		alert("You did not enter your entity kind."); 
		return false;
	}

	if(g_entityname=='')
	{
		alert("You did not enter your entity name."); 
		return false;
	}

	return true;
}

