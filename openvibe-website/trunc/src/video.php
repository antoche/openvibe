<?php

	function argument_collector(&$v_argument_list, &$s_argument_method, &$s_argument_variable)
	{
		if($_POST['q']!='')
		{
			$s_argument_method='post';
			$v_argument_list=$_POST;
			$s_argument_variable='_POST';
		}
		else if($_GET['q']!='')
		{
			$s_argument_method='get';
			$v_argument_list=$_GET;
			$s_argument_variable='_GET';
		}
		else if($HTTP_GET_VARS['q']!='')
		{
			$s_argument_method='get';
			$v_argument_list=$HTTP_GET_VARS;
			$s_argument_variable='HTTP_GET_VARS';
		}
		else
		{
			$s_argument_method='';
			$v_argument_list=Array();
			$s_argument_variable='(none)';
		}
	}

	argument_collector($g_v_argument_list, $g_s_argument_method, $g_s_argument_variable);

	if($g_v_argument_list['q']=="")
	{
		header("Location: index.php");
	}
	else
	{
		$id=$g_v_argument_list['q'];
	}

	if($g_v_argument_list['playlist']=="true")
	{
		$playlist=true;
	}
	else
	{
		$playlist=false;
	}

?>

<html lang="en">
<head>
	<meta http-equiv="Content-Language" content="en" />
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>

	<meta name="author"        content="Yann RENARD INRIA"/>
<!-- <meta name="copyright"     content="Defines copyright information of the document"/> -->
	<meta name="description"   content="OpenViBE software website"/>
	<meta name="distribution"  content="Global"/>
	<meta name="generator"     content="Hand written"/>
	<meta name="keywords"      content="OpenViBE openvibe BCI human brain computer interface neurofeedback neuroscience real time VR virtual reality interaction free opensource"/>
<!-- <meta name="progid"        content="Defines the id of a program used to generate the document"/> -->
<!-- <meta name="rating"        content="Defines webpage rating"/> -->
<!-- <meta name="resource-type" content="Defines the type of the web resource"/> -->
<!-- <meta name="revisit-after" content="Defines the expected update rate of the web resource"/> -->
<!-- <meta name="robots"        content="Defines rules for robots (web crawlers for search engines)"/> -->
<!-- <meta name="others"        content="You can define your own names in a schema"/> -->

	<title>OpenViBE</title>
	<link rel="stylesheet" href="style.css" type="text/css" />

	<script type="text/javascript" src="openvibe.js"></script>

	<script type="text/javascript" src="flash/swfobject.js"></script>
	<script type="text/javascript">

		var player = null;
	
		function playerReady(thePlayer)
		{
			player = window.document[thePlayer.id];
			addListeners();
		}
	
		function addListeners()
		{
			if (player)
			{
				player.addModelListener("TIME", "positionListener");
				player.addViewListener("VOLUME", "volumeListener");
			}
			else
			{
				setTimeout("addListeners()",100);
			}
		}
	
		function createPlayer(url, video_width, video_height, full_video_width, full_video_height)
		{
			var flashvars =
			{
				file:"playlist.xml",
				autostart:"true",
				showdigits: "true",
				showfsbutton:"true",
				displaywidth: "800",
				displayheight: "450",
				width:"800",
				height:"570"
			}
	
			var params =
			{
				allowfullscreen:"true",
				allowscriptaccess:"always"
			}
	
			var attributes =
			{
				id:"player",
				name:"player"
			}

			flashvars.file=url;

			flashvars.displaywidth=video_width;
			flashvars.displayheight=video_height;
			flashvars.width=full_video_width;
			flashvars.height=full_video_height;

			swfobject.embedSWF("flash/player.swf", "placeholder", full_video_width, full_video_height, "9.0.0", false, flashvars, params, attributes);
		}

	</script>

</head>

<body onload="
	<?php
		if($playlist=="true")
		{
			echo "	createPlayer('/medias/".$id."-playlist.xml', '780', '450', '780', '850');";
		}
		else
		{
			echo "	createPlayer('/medias/".$id.".flv', '780', '450', '780', '450');";
		}
	?>
	show_div('video');">

<div id="main">

	<div id="title">
		<center>
		<a name="page-top"></a>
		<table width=95%>
			<tr>
				<td><a href="http://openvibe.inria.fr"><img src="logo.png" /></a></td>
				<td id="title-text">A Software for Brain Computer Interfaces and Real Time Neurosciences</td>
			</tr>
		</table>
		</center>
	</div>

	<script language=javascript type='text/javascript'>
		warning_banner();
	</script>

	<div id="menu">
		<div id="menu-content">
			<ul>
				<li <?php if ($id=='home') echo "id=selected"; ?> ><a href="index.php?q=home">OpenViBE Home</a></li>
			</ul>
			<div class="menu-title">General</div>
			<ul>
				<li <?php if ($id=='about') echo "id=selected"; ?> ><a href="index.php?q=about">About</a></li>
				<li <?php if ($id=='features') echo "id=selected"; ?> ><a href="index.php?q=features">Features</a></li>
				<li <?php if ($id=='made-with') echo "id=selected"; ?> ><a href="index.php?q=made-with">Made with...</a></li>
				<li <?php if ($id=='download') echo "id=selected"; ?> ><a href="index.php?q=download">Download</a></li>
				<li <?php if ($id=='jobs') echo "id=selected"; ?> ><a href="index.php?q=jobs">Job Offers</a></li>
			</ul>
			<div class="menu-title">Help</div>
			<ul>
			<li><a href="documentation/unstable" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">Documentation</a>
				<li><a href="documentation/unstable/Doc_FAQ.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">FAQ</a></li>
				<li <?php if ($id=='support') echo "id=selected"; ?> ><a href="index.php?q=support">Support</a></li>
				<li <?php if ($id=='bci') echo "id=selected"; ?> ><a href="index.php?q=bci">What is a BCI ?</a></li>
				<li <?php if ($id=='medias') echo "id=selected"; ?> ><a href="index.php?q=medias">Medias</a></li>
				<li <?php if ($id=='publications') echo "id=selected"; ?> ><a href="index.php?q=publications">Publications</a></li>
			</ul>
			<div class="menu-title">Contact</div>
			<ul>
				<li <?php if ($id=='contributions') echo "id=selected"; ?> ><a href="index.php?q=contributions">Contributions</a></li>
				<li <?php if ($id=='related') echo "id=selected"; ?> ><a href="index.php?q=related">Related Projects</a></li>
				<li><a href="http://openvibe.inria.fr/forum" class="leaving-link" onclick="window.open(this.href,'openvibe-forum'); return false;">Forum</a></li>
				<li><a href="https://gforge.inria.fr/projects/openvibe" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">INRIA gForge</a></li>
				<li><a href="https://gforge.inria.fr/mail/?group_id=276" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">Mailing Lists</a></li>
				<li><a href="http://openvibe.inria.fr/tracker" class="leaving-link" onclick="window.open(this.href,'openvibe-tracker'); return false;">Bug Tracker</a></li>
<!--				<li><a href="http://www.irisa.fr/bunraku/OpenViBE" class="leaving-link" onclick="window.open(this.href,'openvibe-wiki'); return false;">Research Project</a></li> -->
				<li <?php if ($id=='contact') echo "id=selected"; ?> ><a href="index.php?q=contact">Contact</a></li>
				<li><b>IRC</b> : <span style="font-size: x-small; font-style: italic;">#openvibe</span> on <a href="http://webchat.freenode.net"><span style="font-size: x-small; font-style: italic;">irc.freenode.net</span></a></li>
<!--
				<li><b>Project Leader</b> : Anatole L&eacute;cuyer, INRIA <span style="font-size: x-small; font-style: italic;">(anatole dot lecuyer at irisa dot fr)</span></li>
				<li><b>Lead Software Engineer</b> : Yann Renard, INRIA <span style="font-size: x-small; font-style: italic;">(yann dot renard at irisa dot fr)</span></li>
-->
			</ul>
			<div class="menu-title">Search</div>

			<form action="search.php" method="get" id="search">
				<fieldset>
					<input class="search" type="text" name="q" value="Quick Search..." onfocus="this.value='';" />
				</fieldset>
			</form>
		</div>
	</div>

	<div class="content-container">
	<div class="content" id="video">
	<center>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div id="placeholder">
		<p>To view this video you need JavaScript enabled and the latest Adobe Flash Player installed.</p>
		<p><a href="http://www.adobe.com/go/getflashplayer/">Download the free Flash Player now</a>.</p>
		<p><a href="http://www.adobe.com/go/getflashplayer/"><img src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" alt="Download Flash Player" style="border: none;"></a></p>

		<div id="imgId" style="position:absolute; top:500; left:250">
			<img name='SlideShow' align="center">
			<div id="notId1" style="position:absolute; top:50; left:250">
				<font color="#000000">
				</font>
			</div>
			<div id="notId2" style="position:absolute; top:150; left:250">
				<font color="#000000">
				</font>
			</div>
		</div>
	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	</center>
	</div>
	</div>

	<div class='clear-both'></div>

	<div id="footer">
		<center>
		<table width=95%>
			<tr>
				<td style="text-align:left;"><a href="#page-top">Back to top</a></td>
				<td style="text-align:right;">Last update 2009-05-11</td>
			</tr>
		</table>
		<? echo "<span style='color: #e6be61;'>To access this website, you're using ".getenv("HTTP_USER_AGENT")."</span>"; ?>
		</center>
	</div>
</div>

</body>
