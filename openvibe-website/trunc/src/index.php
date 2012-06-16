<?php

	function register_for_datasets($sName, $sEMail, $sCountry, $sEntity, $sEntityName, $sOtherInfo)
	{
		$email_to="yann.renard@irisa.fr";
		$email_subject="[openvibe-datasets] Registration for download (".$sName.")";
		$email_body="The following user registered for datasets download:\n".
			"name:        ".$sName."\n".
			"email:       ".$sEMail."\n".
			"country:     ".$sCountry."\n".
			"entity:      ".$sEntity."\n".
			"entity-name: ".$sEntityName."\n".
			"other info:\n".$sOtherInfo;
		mail($email_to, $email_subject, $email_body);
	}

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
		$id='home';
	}
	else
	{
		$id=$g_v_argument_list['q'];
	}

	if($id=='datasets-download')
	{
		$l_b_missing_data=false;
		if($_POST['name'] == '') $l_b_missing_data=true;
		if($_POST['email'] == '') $l_b_missing_data=true;
		if($_POST['country'] == '') $l_b_missing_data=true;
		if($_POST['entity'] == '') $l_b_missing_data=true;
		if($_POST['entityname'] == '') $l_b_missing_data=true;

		if($l_b_missing_data==false)
		{
			register_for_datasets($_POST['name'], $_POST['email'], $_POST['country'], $_POST['entity'], $_POST['entityname'], $_POST['otherinfo']);
		}
		else
		{
			$_hostname=gethostbyaddr($_SERVER['REMOTE_ADDR']);
			if($_hostname!='cervelet.irisa.fr')
			{
				header('Location: /?q=datasets');
			}
			else
			{
				echo "NO RELOCATION BECAUSE PAGE WAS ACCESSED FROM cervelet.irisa.fr";
			}
		}
	}

?>

<html lang="en">
<head>
	<meta http-equiv="Content-Language" content="en"/>
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

<?php
echo "<style>".
	"div.media".
	"{".
	"	margin: 0;".
	"	margin-top: 0.5em;".
	"	margin-bottom: 0.5em;".
	"	margin-left: 5em;".
	"	padding: 1em;".
	"	background-color: #f8f8f8;".
	"	border: 1px solid #d0d0d0;";

if(eregi("msie", getenv("HTTP_USER_AGENT"))
|| eregi("internet explorer", getenv("HTTP_USER_AGENT")))
	echo "height: 112px;";
else
	echo "min-height: 112px;";

echo "}</style>";

?>

		<script type="text/javascript" src="openvibe.js"></script>

</head>

<?php

echo "<body onload=\"select_div('".$id."'); show_div('src-download-y'); show_div('win32-installer-download-y'); \">";

?>

<div id="main">

	<div id="title">
		<center>
		<a name="page-top" />
		<table width=95%>
			<tr>
				<td><a href="http://openvibe.inria.fr"><img src="logo.png" alt="OpenViBE logo"/></a></td>
				<td>OpenViBE Software for Brain Computer Interfaces and Real Time Neurosciences</td>
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
				<li <?php if ($id=='home') echo "id=selected"; ?> ><a href="?q=home">OpenViBE Home</a></li>
			</ul>
			<div class="menu-title">General</div>
			<ul>
				<li <?php if ($id=='about') echo "id=selected"; ?> ><a href="?q=about">About</a></li>
				<li <?php if ($id=='features') echo "id=selected"; ?> ><a href="?q=features">Features</a></li>
				<li <?php if ($id=='made-with') echo "id=selected"; ?> ><a href="?q=made-with">Made with...</a></li>
				<li <?php if ($id=='download') echo "id=selected"; ?> ><a href="?q=download">Download</a></li>
				<li <?php if ($id=='jobs') echo "id=selected"; ?> ><a href="?q=jobs">Job Offers</a></li>
			</ul>
			<div class="menu-title">Help</div>
			<ul>
			<li><a href="documentation/unstable" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">Documentation</a>
				<li><a href="documentation/unstable/Doc_FAQ.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">FAQ</a></li>
				<li <?php if ($id=='support') echo "id=selected"; ?> ><a href="?q=support">Support</a></li>
				<li <?php if ($id=='bci') echo "id=selected"; ?> ><a href="?q=bci">What is a BCI ?</a></li>
				<li <?php if ($id=='medias') echo "id=selected"; ?> ><a href="?q=medias">Medias</a></li>
				<li <?php if ($id=='publications') echo "id=selected"; ?> ><a href="?q=publications">Publications</a></li>
			</ul>
			<div class="menu-title">Contact</div>
			<ul>
				<li <?php if ($id=='contributions') echo "id=selected"; ?> ><a href="?q=contributions">Contributions</a></li>
				<li <?php if ($id=='related') echo "id=selected"; ?> ><a href="?q=related">Related Projects</a></li>
				<li><a href="http://openvibe.inria.fr/forum" class="leaving-link" onclick="window.open(this.href,'openvibe-forum'); return false;">Forum</a></li>
				<li><a href="https://gforge.inria.fr/projects/openvibe" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">INRIA gForge</a></li>
				<li><a href="https://gforge.inria.fr/mail/?group_id=276" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">Mailing Lists</a></li>
				<li><a href="http://openvibe.inria.fr/tracker" class="leaving-link" onclick="window.open(this.href,'openvibe-tracker'); return false;">Bug Tracker</a></li>
<!--				<li><a href="http://www.irisa.fr/bunraku/OpenViBE" class="leaving-link" onclick="window.open(this.href,'openvibe-wiki'); return false;">Research Project</a></li> -->
				<li <?php if ($id=='contact') echo "id=selected"; ?> ><a href="?q=contact">Contact</a></li>
				<li><b>IRC</b> : <span style="font-size: x-small; font-style: italic;">#openvibe</span> on <a href="http://webchat.freenode.net"><span style="font-size: x-small; font-style: italic;">irc.freenode.net</span></a></li>
<!--
				<li><b>Project Leader</b> : Anatole L&eacute;cuyer, INRIA <span style="font-size: x-small; font-style: italic;">(anatole dot lecuyer at irisa dot fr)</span></li>
				<li><b>Lead Software Engineer</b> : Yann Renard, INRIA <span style="font-size: x-small; font-style: italic;">(yann dot renard at irisa dot fr)</span></li>
-->
			</ul>
			<div class="menu-title">Share this</div>
			<ul><li><span style="font-size: x-small; font-style: italic;">This website is licenced under <b>CC-BY-SA</b></span></li></ul>
			<td><a href="http://creativecommons.org/licenses/by-sa/3.0/legalcode" class="leaving-link" onclick="window.open(this.href,'cc-by-sa'); return false;"><img src="cc-by-sa.png" /></a></td>
			<br>
			<div class="menu-title">Search</div>

			<form action="search.php" method="get" id="search">
				<fieldset>
					<input class="search" type="text" name="q" value="Quick Search..." onfocus="this.value='';" />
				</fieldset>
			</form>
		</div>
	</div>

	<div class="content-container">

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="home">

		<h1>Welcome</h1>

		<p>Welcome, you have reached the home of the OpenViBE software.</p>
		<p>OpenViBE enables to design, test and use Brain-Computer Interfaces. Brain-Computer Interfaces (<a href="?q=bci">BCI</a>)
		are communication systems that enable users to send commands to computers only by means of brain activity.</p>
		<p>Broadly speaking, OpenViBE can be used in many real-time neurosciences applications.</p>
		<p>This video will help you understand what OpenViBE is and how it can be used :</p>

		<script language=javascript type='text/javascript'>
			video("video.php?q=video-openvibe-introduction-en", "openvibe-video-demonstrators.png", "This 10mn video is a global overview of what a Brain Computer Interface is and how OpenViBE was used to interact with virtual environments.");
			video("video.php?q=video-openvibe-promo-long-en", "openvibe-video-introduction.png", "This 4mn video is a quick overall presentation of what our software is an how it can be used to quickly build Brain Computer Intefaces.");
			/* http://videotheque.inria.fr/videotheque/VoirDoc.do?leMot=615 */
		</script>

		<p>OpenViBE is free and open source. Don't hesitate to download it and <a href="?q=download">give it a try.</a></p>

		<h1>OpenViBE Users Around The World</h1>

		<p>The following map shows the different places where people use OpenViBE !</p>
		<center><img class="fixed-screenshot" src=openvibe-users-cartography.png></img></center>

<!-- -->
		<h1>Job Offers</h1>

		<p><span style="color: red; font-weight: bold;">There are opened positions to work on and with OpenViBE...</span></p>
		<p>Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>
<!-- -->

<!-- -->
		<h1>Latest "Made With OpenViBE" Entry</h1>

		<p>Dieter Devlaminck, researcher at <a href=http://www.ugent.be/en>Ghent University</a>, reported his recent results with OpenViBE : controlling a robotic arm
		thanks to P300 signals. What he did with OpenViBE was basically to derive the xDAWN based P300 signals speller scenario. Each letter in the grid allows to
		send a specific command to the robot. He posted a video demonstrating those results.</p>

		<div style="padding:1em; margin-top: 1em; margin-bottom: 1em; margin-left: 4em; margin-right: 4em; border: 1px solid #D0D0D0; background-color: #F0F0F0; text-align: center; -moz-border-radius: 0.5em"><object width="400" height="300"><param name="allowfullscreen" value="true" /><param name="allowscriptaccess" value="always" /><param name="movie" value="http://vimeo.com/moogaloop.swf?clip_id=8841995&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;show_portrait=0&amp;color=&amp;fullscreen=1" /><embed src="http://vimeo.com/moogaloop.swf?clip_id=8841995&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;show_portrait=0&amp;color=&amp;fullscreen=1" type="application/x-shockwave-flash" allowfullscreen="true" allowscriptaccess="always" width="400" height="300"></embed></object><p><a href="http://vimeo.com/8841995">Robotic toy arm controlled through brain-computer interface</a> from <a href="http://vimeo.com/user2212371">Dieter Devlaminck</a> on <a href="http://vimeo.com">Vimeo</a>.</p></div>

		Warm thanks to Dieter Devlaminck for reporting this work to us !
<!-- -->

		<p>If you want to see more <i>Made With OpenViBE</i> applications, visit the <a href="?q=made-with">Made With...</a> section</p>

		<h1>News</h1>
		
		<h2>New release of OpenViBE 0.12.0 is now available<br><span style="font-size:x-small">13/10/2011</span></h2>

		<p>The OpenViBE development team is happy to announce the release of *OpenViBE* 0.12.0 "Autumn edition", available for download at the <a href="http://openvibe.inria.fr/?q=download">download page</a> 

		<p>This release brings many new features, including one community contribution :</p>
		
		<ul>
		<li><b>Nicolas Tarrin, Mathieu Goyat & Gelu Ionescu</b> contributed a <b>stream synchronization box</b>, that can synchronize in OpenViBE several streams coming from several linked devices.</li>
				
		<li>The <b>time</b>is now printed in every logs (console, file, etc.) in seconds. User can switch between this display and the previous (in 32:32 time and/or hexadecimal) at will using configuration tokens. The time precision can also be set.</li>
		<li>The <b>Emotiv EPOC driver</b> is now included in the Windows Installer! It still requires the user to specify where to find the Emotiv Research Edition SDK in order to run correctly.</li>
		<li>The <b>g.Tec g.USBAmp driver</b> benefits from a major update that add more amplifier functionalities: ability to connect to <b>common ground and reference</b>, activation of <b>notch and band-pass</b> filters.</li>
		<li>The <b>Matlab Filter box<b> is now compiled by default and included in the release. This box is still unstable, but has been fully reworked in order to run on every machine, as long as Matlab (version 32bits) is installed.</li>
		<li>... and more new features and bugfixes !</li>

		Detailed changelog can be found in this <a href="http://openvibe.inria.fr/forum/viewtopic.php?f=1&t=578&p=2829#p2830">forum post</a>

		Many thanks to Nicolas, Mathieu and Gelu for their contributions! If you are interested in joining the effort feel free to contact us!
		
		<h2>New release of OpenViBE 0.11.0 is now available<br><span style="font-size:x-small">11/08/2011</span></h2>

		<p>The OpenViBE development team is happy to announce the release of *OpenViBE* 0.11.0 "Summer edition", available for download at the <a href="http://openvibe.inria.fr/?q=download">download page</a> 

		<p>This release brings many exciting features, including three community contributions :</p>
		
		<ul>
		<li><b>Aurélien Van Langhenhove</b> contributed a driver based on OpenAL to acquire *sound signal* from microphone input.</li>
		<li><b>Joan Fruitet</b> patched the Matlab Filter box, by fixing bugs and adding stimulation output.</li>
		<li><b>Olivier Rochel</b> contributed bugfix patches, for matlab box and skeleton-generator. </li>
		
		<li>Motor-imagery and SSVEP scenarios now use *CSP* for higher performances.</li>
		<li><b>Emotiv driver</b> was updated to make *gyroscope* values available.</li>
		<li>Signal Concatenation box has been rewritten for <b>fast file concatenation</b>.</li>
		<li>The developer tool <b>Skeleton-Generator</b> benefited from a major update.</li>
		<li>The <b>Matlab</b> box outputs stimulations.</li>
		<li>Channel Selector and Reference Channel feature *smart* channel selection (names, indexes, ranges).</li>
		<li>... and more new features detailed below.</li>

		Detailed changelog can be found in this <a href="http://openvibe.inria.fr/forum/viewtopic.php?f=1&t=532">forum post</a>

		Many thanks to Aurélien, Joan and Olivier for their contributions! If you are interested in joining the effort feel free to contact us!

		<p><b>Detailed feature list</b></p>

		<ul>

		<p><i>COMMUNITY</i></p>
		<li>+ Aurélien Van Langhenhove contributed a driver based on OpenAL to acquire sound signal from microphone input.</li>
		<li>+ Joan Fruitet contributed a patch for the Matlab filter box, by fixing bugs and adding stimulation output.</li>
		<li>+ Olivier Rochel contributed patches for the skeleton-generator and the matlab box.</li>
		
		<p><i>SUPPORT</i></p>
		<li>+ OpenViBE is now officially supported on Fedora 15, Ubuntu 11.04 and Windows 7 64bit.</li>
		
		<p><i>APPLICATIONS</i></p>
		<li>* The Emotiv EPOC driver now acquires values from gyroscope sensors (new option in the driver properties).</li>
		<li>* The Player in the OpenViBE Designer can now be controlled with keyboard shortcuts (F5: stop, F6: one-step, F7: play, F8: fast-forward)</li>
		<li>* Skeleton Generator benefited from a major update, and can produce box skeleton with Listeners, Codecs, Flags, etc.</li>
		
		<p><i>PLUGINS</i></p>
		<li>+ New Reference Channel and Channel Selector boxes. The older boxes are now deprecated. The channel selection is smarter (name or 1-based index, 'X:Y' for channel range).</li>
		<li>+ New Signal Concatenation box. The older box is now deprecated. The concatenation is much faster, and end-of-file is detected automatically through a time-out.</li>
		<li>+ New Stream Switch box that copies its input chunks on one of its outputs. The active output is selected with defined stimulations.</li>
		<li>* CSV file writer can receive 1 or 2-dimensional matrices</li>
		<li>* Matrix display can receive 1 or 2-dimensional matrices</li>
		<li>* The Matlab filter box has now a Stimulation output, and the Matlab messages are redirected to the OpenViBE console (warnings, errors, disp, etc.)</li>
		<li>* Classifier Trainer outputs standard deviation along with classification percentage on the k-fold test. k-fold test now randomize the input data prior to testing by default.</li>
		<li>* CSP and xDAWN spatial filter trainers output a stimulation OVTK_StimulationId_TrainCompleted after a successful training.</li>
		<li>* Classifier Accuracy visualization box can now show score and/or percentages, or none of them.</li>
		
		<p><i>RESOURCES</i></p>
		<li>+ New set of scenarios for a Motor-Imagery based BCI using CSP spatial filters, and demonstrating how to measure classifier performances offline or online (confusion matrix, overall performance).</li>
		<li>+ Documentation for the Matlab Filter box.</li>
		<li>+ Documentation for the Display-Cue visualisation box.</li>
		<li>* SSVEP scenarios now make use of CSP spatial filters for better performances.</li>
		<li>* Acquisition Server documentation updated to latest GUI modification (division driver properties/server settings)</li>

		</ul>

		<h2>A patch for windows installer (0.10.1) is available<br><span style="font-size:x-small">2011/13/04</span></h2>

		<p>A small glitch during the creation of the installer made Brain Products First-Amp and V-Amp unavailable. The issue is now fixed.</p>

		<p>You can download the new installer at our usual <a href="http://openvibe.inria.fr/?q=download">download page</a>. Source releases are unaffected.</p>
			
		<h2>New release of OpenViBE 0.10.0 is now available<br><span style="font-size:x-small">2011/12/04</span></h2>

		<p>The OpenViBE development team is happy to announce the release of *OpenViBE* 0.10.0 "Easter edition", available for download at the <a href="http://openvibe.inria.fr/?q=download">download page</a> 

		<p>This release brings many exciting features, including two community contributions :</p>
		
		<ul>
		<li><b>Dieter Devlaminck</b> contributed a box for training spatial filters using the *CSP* algorithm.</li>
		<li><b>Joan Fruitet</b> contributed a box which can display images, such as instructions, upon receiving stimulations.</li>
		
		<li>Scenarios for a new <b>SSVEP paradigm</b> are available.</li>
		<li><b>Neurosky Mindset</b> driver was updated to use SDK v2.1 and now supports blink detection.</li>
		<li>Designer now features a <b>console</b>, making error messages easier to catch.</li>
		<li>A new <b>VRPN Client</b> box is available, so OpenViBE can receive messages from external applications.</li>
		<li>A new, unstable, <b>CSV Reader</b> box is available.</li>
		<li>A new box for <b>Sound stimulation</b>, based on OpenAL, is available.</li>
		<li>New <b>tools</b> for box developers.</li>
		<li>... and more new features detailed below.</li>

		Detailed changelog can be found in this <a href="http://openvibe.inria.fr/forum/viewtopic.php?f=1&t=460">forum post</a>

		Many thanks to Dieter and Joan for their contributions! If you are interested in joining the effort feel free to contact us!

		<p><b>Detailed feature list</b></p>

		<ul>

		<li>+ Dieter Devlaminck contributed a box for training spatial filters using CSP algorithm</li>
		<li>+ Joan Fruitet contributed a box which can display user selected images upon receiving stimulations</li>
		<li>+ Added templates for easy encoder/decoder creation and manipulation</li>
		<li>+ We added scenarios for SSVEP BCIs along with an application for SSVEP stimulation</li>
		<li>+ We added new drivers for raw file reading and telnet raw reading to the acquisition server</li>
		<li>+ You can now choose whether to activate, deactivate or force the drift correction in the acquisition server</li>
		<li>+ We added support for triggers in the TMSiRefa32B and MicromedSystemPlusEvolution drivers</li>
		<li>* Neurosky Mindset driver was updated to use SDK v2.1</li>
		<li>+ Neurosky Mindset driver now supports blink detection</li>
		<li>+ The designer can now read scenarios from standard input</li>
		<li>+ A collapsible console showing the output messages has been added to the designer</li>
		<li>+ An optional pop-up window is now displayed when there are errors during the execution of a scenario</li>
		<li>+ Multiple files can now be opened at once in the designer</li>
		<li>+ The designer now supports several new keyboard shortcuts
			<ul>
			  <li>F2 - rename all selected boxes</li>
			  <li>Ctrl+A - select all boxes in the current scenario</li>
			  <li>Ctrl+W - close the currently opened scenario</li>
			</ul>
		</li>
		<li>* Spatial filter can now work with signals, spectrums and streamed matrices</li>
		<li>+ A new VRPN Client box is available so OpenViBE can receive messages from external applications</li>
		<li>+ A new, unstable, CSV Reader box is available</li>
		<li>+ Lua stimulator now has access to the log manager</li>
		<li>+ A new box for Sound stimulation, based on OpenAL, is available. The old SoundStimulation box is now deprecated.</li>
		<li>+ Moving average box can now use two new methods - Cumulative Average and Immediate Moving Average</li>
		<li>* A lot of bugfixes detailed in the post on the forum</li>

		</ul>


		<h2>
		OpenViBE at Techfest 2011, Mumbai (India).<br><span style="font-size:x-small">2011/01/7</span></h2>
		<p>
		The OpenViBE developer team is representing the INRIA at <a href=http://techfest.org>Techfest 2011</a>, probably the largest event in Asia related to new technologies. This event takes place at the India Institute of Technology (IIT-Bombay) in Mumbai, from 7th to 9th january. </p>
		<p>We will present the OpenViBE software and show some nice demonstrations of BCI using OpenViBE. Rendez vous in the international exhibitor's pavilion !</p>
		
		<h2>New release of OpenViBE 0.9.0 is now available<br><span style="font-size:x-small">2010/12/28</span></h2>

		<p>New release of OpenViBE 0.9.0 "Christmas edition" is now available for download at the <a href=http://openvibe.inria.fr/?q=download>download</a> page.</p>
		<p>In this new release, you will find the following modifications (+ for adds, * for modifications, - for removes) :</p>
		<ul>
			<li> + We added an EGI Netamps driver</li>
			<li> + We added multi amp support for Brain Products BrainAmp series</li>
			<li> * Emmanuel Maby stabilized the Brain Products BrainVision Recorder driver (the driver is now stable)</li>
			<li> * The acquisition server now remembers the last device you used</li>
			<li> * The designer now remembers the last opened scenarios</li>
			<li> * The designer can be launched from command line hiding the GUI, loading/runing a specific scenario etc..</li>
			<li> * Each box setting can now be configured with the configuration manager</li>
			<li> * We stabilized lua box with a more complete API (the box is now stable)</li>
			<li> + We added a VRPN Button client box</li>
		</ul>
		<p>We want to thank again all the forum and bug tracker participants who help in making the software better every day.</p>
		<p>Feel free to join us and to <a href=?q=contributions>contribute</a> as others are doing... !</p>
		<br>
		<p>Here is a snapshot of what we are currently doing and what you can expect from the next release :</p>
		<ul>
			<li> + Sample scenarios and VR demos should unable the use of SSVEP</li>
		</ul>

		<h2>One Post-Doc Position at Hopital Raymond Poincare, France<br><span style="font-size:x-small">2010/12/03</span></h2>

		<p>There are opened positions to work on and with OpenViBE at Hopital Raymond Poincare. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>Three Positions at INRIA Sophia Antipolis, France<br><span style="font-size:x-small">2010/12/03</span></h2>

		<p>There are opened positions to work on and with OpenViBE at INRIA Sophia Antipolis. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>New release of OpenViBE 0.8.0 is now available<br><span style="font-size:x-small">2010/10/01</span></h2>

		<p>New release of OpenViBE 0.8.0 "Autumn edition" is now available for download at the <a href=http://openvibe.inria.fr/?q=download>download</a> page.</p>
		<p>In this new release, you will find the following modifications (+ for adds, * for modifications, - for removes) :</p>
		<ul>
			<li> + Matthieu Goyat and Guillaume Lio contributed a box to join multiple signal streams in a single stream</li>
			<li> + Matthieu Goyat contributed an update to the Sound Player box to make it work on Windows</li>
			<li> + Yann Renard contributed a Brainamp Series driver using Brain Products low level API</li>
			<li> + We added a driver for the Emotiv EPOC</li>
			<li> + We implemented impedance check on the TMSi Refa driver</li>
			<li> + We updated the Brain Products VAmp driver to support FirstAmp8</li>
			<li> + We multithreaded the acquisition server for better performance and synchronization</li>
			<li> + We significantly enhanced drift management in the acquisition server</li>
			<li> + We added a driver seketon generator for driver developers</li>
			<li> + We restored the handball VR application for interaction from the motor imagery BCI</li>
			<li> + We added a box to compute the power of a spectrum frequency band</li>
			<li> + We added a box to rename signal channels</li>
			<li> + We added a SVM classifier (based on libsvm)</li>
			<li> * We updated the ModularEEG driver</li>
			<li> * We updated the temporal filter box to correct errors - should be very close to MATLAB now</li>
			<li> * We moved from libglade to gtk_builder allowing upgrading GTK</li>
			<li> * We updated windows dependencies to it++ 4.0.7, vrpn 7.26, boost 1.42, cmake 2.8.2, gtk 2.16.6</li>
			<li> * We updated the online documentation and tutorials</li>
			<li> * We updated many documentation pages</li>
			<li> * We fixed lots of bugs !</li>
		</ul>
		<p>We want to thank Matthieu Goyat and Guillaule Lui for their contributions.</p>
		<p>We also want to thank again all the forum and bug tracker participants who help in making the software better every day.</p>
		<p>Feel free to join us and to <a href=?q=contributions>contribute</a> as Matthieu, Guillaume and others are doing... !</p>
		<br>
		<p>Here is a snapshot of what we are currently doing and what you can expect from the next release :</p>
		<ul>
			<li> + An updated/stable Brain Products driver through Vision Recorder should be available</li>
			<li> + An EGI acquisition driver through AmpServer Pro should be available</li>
			<li> + Sample scenarios and VR demos should unable the use of SSVEP</li>
			<li> + Visual C++ Express 2010 will definitely be supported :)</li>
		</ul>
		<p><u>Note</u>: due to licence limitations, the Emotiv EPOC driver is not included in the
		pre-compiled package for Windows. See <a href=http://openvibe.inria.fr/documentation/0.8.0/Doc_FAQ.html#Doc_FAQ_Emotiv>this FAQ entry</a>
		for details about how to build this driver.</p>

		<h2>New release of OpenViBE 0.7.0 is now available<br><span style="font-size:x-small">2010/07/08</span></h2>

		<p>New release of OpenViBE 0.7.0 "Summer edition" is now available for download at the <a href=http://openvibe.inria.fr/?q=download>download</a> page.</p>
		<p>In this new release, you will find the following modifications (+ for adds, * for modifications, - for removes) :</p>
		<ul>
			<li> + Matthieu Goyat contributed the Univariate Statistics box</li>
			<li> + We added a new driver : Neurosky Mindset</li>
			<li> + We added the possibility to comment scenarios in the designer</li>
			<li> + We added some control over the player execution from the boxes</li>
			<li> + We added a box that is able to control the player execution</li>
			<li> + We added more configuration possibilities with the configuration file tokens</li>
			<li> + We added many small "user oriented" features in the designer such as left/right scroll with mouse wheel and better overview of renamed boxes</li>
			<li> + We added a lot of great features to the Simple DSP box - see documentation and sample scenarios</li>
			<li> * The acquisition server is now smarter on CPU use</li>
			<li> * We reviewed and commented all the box-tutorials and xDAWN p300 speller / motor imagery scenarios</li>
			<li> * We alphabetically sorted the stimulations for quicker configuration of the boxes</li>
			<li> * We restored automatic filename extension/filtering when loading/saving a scenario</li>
			<li> * We corrected a bug on the k-fold test in the classifier trainer</li>
			<li> * We updated the online documentation and tutorials</li>
			<li> * We updated many documentation pages</li>
			<li> * We fixed lots of bugs !</li>
		</ul>
		<p>We want to thank Matthieu Goyat for his contribution to the Univariate Statistics box.</p>
		<p>We also want to thank all the forum and bug tracker participants who help in making the software better every day.</p>
		<p>Feel free to join us and to <a href=?q=contributions>contribute</a> as Matthieu and others are doing... !</p>
		<br>
		<p>Here is a snapshot of what we are currently doing and what you can expect from the next release :</p>
		<ul>
			<li> + An EGI acquisition driver</li>
			<li> + More tools dedicated to plugins developpers (base classes, code generators etc...)</li>
			<li> + More tools dedicated to authors / designer users</li>
			<li> + Hopefully, we should support Visual C++ express 2010 and Windows 7</li>
			<li>...</li>
		</ul>

		<h2>OpenViBE Tutorial Slides<br><span style="font-size:x-small">2010/06/07</span></h2>

		<p>We are back from the <a href=http://bcimeeting.org/2010>BCI Meeting</a> and really enjoyed the event. Attenders had the opportunity to come to the OpenViBE tutorial special session...</p>
		<p>You can now download the tutorial slides from the dedicated <a href=http://bcimeeting.org/wiki/tiki-index.php?page=Session+1>wiki page</a> of the BCI meeting website !</p>

		<h2>One Software Engineer Position at Hopital Raymond Poincare, France<br><span style="font-size:x-small">2010/06/07</span></h2>

		<p>There are opened positions to work on and with OpenViBE at Hopital Raymond Poincare. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>OpenViBE demonstrator at La Cité des Sciences et de l'Industrie, Paris.<br><span style="font-size:x-small">2010/05/07</span></h2>

		<p>A show room dedicated to innovative technologies, the Tech'Gallery, is now opened at <a href=http://www.cite-sciences.fr/en/cite-des-sciences>La Cité des Sciences et de l'Industrie</a> (the City of Sciences and Industry) in Paris.</p>
		<p>In association with the laboratory <a href=http://www.lutin-userlab.fr/accueil>Lutin Userlab</a> and the EEG manufacturer <a href=http://www.gtec.at/>g.tec</a>, we present OpenViBE and the "Use-The-Force" demonstrator.</p>
		<p>Don't hesitate, come and try ! Exhibition is opened every day (9:30 AM - 6 PM), visitors can try the demonstrator on tuesday and saturday (2 PM - 4 PM).</p>

		<!--
		<h2>Datasets<br><span style="font-size:x-small">2010/05/07</span></h2>

		<p>During the past experiments, we recorded several datasets under different circumstances. These datasets can be downloaded for free in the hope that they can help research on signal processing. They have all been recorded using OpenViBE.</p>
		<p>They are available on the <a href=http://openvibe.inria.fr/?q=datasets>datasets</a> page.</p>
		-->
		<h2>OpenViBE publications in Presence Teleoperators & Virtual Environments<br><span style="font-size:x-small">2010/05/07</span></h2>

		<p> The latest edition of the journal <a href="http://www.mitpressjournals.org/toc/pres/19/1">Presence</a> includes two "OpenViBE" publications.
		<ul>
			<li>Y. Renard, F. Lotte, G. Gibert, M. Congedo, E. Maby, V. Delannoy, O. Bertrand, A. Lécuyer, "OpenViBE: An Open-Source Software Platform to Design, Test and Use Brain-Computer Interfaces in Real and Virtual Environments", Presence : teleoperators and virtual environments, vol. 19, no 1, 2010 (in press) (<a href="http://www.hal.inserm.fr/docs/00/47/71/53/PDF/Renard_et_al_2010_draft.pdf"> pdf version</a>)</li>
			<li>F. Lotte, A. Van Langhenhove, F. Lamarche, T. Ernest, Y. Renard, B. Arnaldi, A. Lécuyer, Exploring Large Virtual Environments by Thoughts using a Brain-Computer Interface based on Motor Imagery and High-Level Commands, Presence: teleoperators and virtual environments, vol. 19, no. 1, 2010 (in press)(<a href="http://hal.archives-ouvertes.fr/docs/00/44/56/14/PDF/presence_2010_Museum_revisedDraft.pdf"> pdf version</a>)</li>
		</ul>
		<p>If you want to cite OpenViBE in your work, please refer to the first article. </p>

		<h2>New release of OpenViBE 0.6.0 is now available<br><span style="font-size:x-small">2010/04/14</span></h2>

		<p>New release of OpenViBE 0.6.0 "Spring edition" is now available for download at the <a href=http://openvibe.inria.fr/?q=download>download</a> page.</p>
		<p>In this new release, you will find the following modifiactions (+ for adds, * for modifications, - for removes) :</p>
		<ul>
			<li> + Lucie Daubigney contributed a new driver : g.Tec's gMobiLab+ acquisition device for Linux</li>
			<li> + We added a new driver : Micromed SD LTM (through SystemPlus Evolution)</li>
			<li> + We added impedance check to the drivers that could support it</li>
			<li> + We updated the sample scenarios</li>
			<li> + We added a Confusion Matrix computation box</li>
			<li> + We added a Matrix Display box
			<li> + We added a Lua stimulation box</li>
			<li> + We reenabled VR demos as default</li>
			<li> * We enhanced the visuals of the VR demos and added a few stats to make the experience more entertaining</li>
			<li> * We refactored the acquisition server so the server itself runs in a separate thread for better performances</li>
			<li> * We corrected some bugs on the acquisition server that caused desynchronisations</li>
			<li> * We moved to Ogre 1.7 for 3D display</li>
			<li> * We moved to Boost 1.41</li>
			<li> * We updated the online documentation and tutorials</li>
			<li> * We updated many documentation pages</li>
			<li> * We fixed lots of bugs !</li>
			<li> - We removed OpenMASK / OBT dependency</li>
		</ul>
		<p>We want to thank Lucie Daubigney for her contribution to the g.Mobilab+ driver. Feel free to join us and to <a href=?q=contributions>contribute</a> as Lucie and others are doing... !</p>
		<br>
		<p>Here is a snapshot of what we are currently doing and what you can expect from the next release :</p>
		<ul>
			<li> + A Neuroscan acquisition driver</li>
			<li> + A MitsarEEG acquisition driver</li>
			<li> + An EGI acquisition driver</li>
			<li> + More tools dedicated to plugins developpers (base classes, code generators etc...)</li>
			<li> + More tools dedicated to authors / designer users</li>
			<li> + A more general matrix operations box</li>
			<li>...</li>
		</ul>

		<h2>18 Months Software Engineer Position Opened at CEA/Leti<br><span style="font-size:x-small">2010/03/18</span></h2>

		<p>There are opened positions to work on and with OpenViBE at CEA/Leti. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>Two Years Post-Doc Positions Opened at INRIA<br><span style="font-size:x-small">2010/01/08</span></h2>

		<p>There are opened positions to work on and with OpenViBE at INRIA. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>New release of OpenViBE 0.5.0 is now available<br><span style="font-size:x-small">2009/12/18</span></h2>

		<p>New release of OpenViBE 0.5.0 "Christmas edition" is now available for download at the <a href=http://openvibe.inria.fr/?q=download>download</a> page.</p>
		<p>In this new release, you will find the following modifiactions (+ for adds, * for modifications, - for removes) :</p>
		<ul>
			<li> + We added a new driver : brain products vAmp acquisition device</li>
			<li> + We added a new driver : g.Tec's gUSBamp acquisition device</li>
			<li> + We updated the sample scenarios</li>
			<li> + We propose a new P300-based pipeline based on the xDAWN algorithm</li>
			<li> + We added an online comparison of different processing pipelines performance (e.g. multiple classifiers)</li>
			<li> * We updated the dependencies installation for windows so that Visual C++ Runtime gets included in the package</li>
			<li> * We updated the dependencies installation for windows so that the lack of internet connection does not abort the installation when DirectX is not present</li>
			<li> * We updated the online documentation and tutorials</li>
			<li> * We fixed lots of bugs !</li>
		</ul>
		<p>Feel free to join us and to contribute as others are doing...</p>

		<h2>Release policy of OpenViBE<br><span style="font-size:x-small">2009/10/23</span></h2>

		<p>Starting from release 0.4.0, we decided that OpenViBE would be released every three months. You can expect a 0.5.0 release
		around christmas, 0.6.0 release by the end of march etc...</p>

		<h2>New release of OpenViBE 0.4.0 is now available<br><span style="font-size:x-small">2009/10/23</span></h2>

		<p>New release of OpenViBE 0.4.0 is now available for download at the <a href="http://openvibe.inria.fr/?q=download">download</a> page.</p>
		<p>In this new release, you will find the following modifiactions (+ for adds, * for modifications, - for removes) :</p>
		<ul>
			<li>+ Mr. Christoph Veigl contributed and added a new driver for OpenEEG Modular EEG / Monolith EEG</li>
			<li>+ We added a new driver : g.Tec's gUSBamp acquisition device</li>
			<li>+ We added a new P300-based entertaining application called "Magic Card" + We added tooltips for new users :)</li>
			<li>+ We updated the sample scenarios</li>
			<li>* We propose a stabilized P300-based pipeline</li>
			<li>* We made the development of new classifiers easier thanks to base algorithms</li>
			<li>* We updated the dependencies installation script for linux so that it uses native packages instead of compiling everything from scratch</li>
			<li>* We updated the dependencies installation for windows so that DirectX and Visual C++ Runtime gets installed automatically if needed</li>
			<li>* We updated the online documentation and tutorials</li>
			<li>* We fixed lots of bugs !</li>
			<li>- The VR demo are no more built by default as OpenMASK is not compiling on recent Linux distributions</li>
		</ul>
		<ul>
			<li>+ We added several tooltips for new users :)</li>
			<li>+ We added a k-fold test in the classifier trainer box</li>
			<li>+ We added a functionnality to load/save channel names in the acquisition server</li>
			<li>+ We enabled the voting classifier box to vote either on streamed matrix or on stimulations</li>
			<li>+ We added a frequency band selector box</li>
			<li>+ We added a signal decimation box</li>
			<li>+ We added a CSV file writer box (text based)</li>
			<li>* We changed the way chanels can be selected in the signal display, power spectrum display and time frequency map display</li>
			<li>* We reimplemented the common average reference box</li>
		</ul>
		<p>Here is a snapshot of what we are currently doing and what you can expect from the next release :</p>
		<ul>
			<li>+ A BrainProducts V-Amp acquisition driver</li>
			<li>+ A Neuroscan acquisition driver</li>
			<li>+ A MitsarEEG driver</li>
			<li>+ Online comparison of different processing pipelines performance (e.g. multiple classifiers)</li>
			<li>+ More documentation on the sample scenarios</li>
			<li>* GUI definition will move from glade to gtk-builder</li>
			<li>* VR demos will move from OpenMASK to native Ogre3D</li>
		</ul>
		<p>We want to thank Christoph Veigl for his quick and efficient contribution. Feel free to join us and to contribute as Christoph and others are doing...</p>

		<h2>Two new fulltime OpenViBE enginners at INRIA<br><span style="font-size:x-small">2009/10/01</span></h2>

		<p>Warm welcome to Laurent Bonnet and Batpiste Payan who join the INRIA team working on OpenViBE.</p>
		<p>They will be present for at least two years, participating in the development and support of the platform.</p>

		<h2>OpenViBE bug tracker switches to Mantis<br><span style="font-size:x-small">2009/07/24</span></h2>

		<p>The bug/issue tracker of the INRIA gForge was quite hard to use so we decided to switch to the excellent <a href="http://www.mantisbt.org" class="leaving-link" onclick="window.open(this.href); return false;">Mantis</a>
		bug tracker. There is an anonymous access available. So you can submit your <a href="http://openvibe.inria.fr/tracker/bug_report_page.php" class="leaving-link" onclick="window.open(this.href,'openvibe-tracker'); return false;">bug report or feature request</a>
		in a couple of clicks. Don't hesitate to use this tool, it can be very helpfull for us and the other users.</p>

		<h2>One Software Engineer and one Post-Doc Positions Opened at Gipsa-lab<br><span style="font-size:x-small">2009/06/17</span></h2>

		<p>There are opened positions to work on and with OpenViBE at Gipsa-lab. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>OpenViBE demonstration at the BBCI workshop<br><span style="font-size:x-small">2009/06/16</span></h2>

		<p>OpenViBE will be presented and demonstrated at the BBCI workshop in Berlin, Germany
		on july 8, 9 and 10th (<a href="http://bbci.agilemeetings.com" class="leaving-link" onclick="window.open(this.href,'openvibe-null')";>http://bbci.agilemeetings.com</a>).</p>
		<p>We hope to meet you there.</p>

		<h2>Two Software Engineer and one PhD Thesis Positions Opened at INRIA<br><span style="font-size:x-small">2009/06/08</span></h2>

		<p>There are opened positions to work on and with OpenViBE at INRIA. Check the <a href="?q=jobs">Job Offers</a> page for more details.</p>

		<h2>First official release<br><span style="font-size:x-small">2009/05/05</span></h2>

		<p>We are proud to announce the first official release of OpenViBE. Visit the
		<a href="?q=download">download</a> section to obtain your free copy.</p>
		<p>We are waiting for your feedback! So feel free to contact us, be it on the
		<a href="http://openvibe.inria.fr/forum" class="leaving-link" onclick="window.open(this.href,'openvibe-forum'); return false;">forum</a>, on the
		<a href="https://gforge.inria.fr/mail/?group_id=276" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">mailing lists</a>, or on the
		IRC : <i>#openvibe</i> on <i>irc.freenode.net</i>.</p>
		<p>And don't forget to fill in a <a href="http://openvibe.inria.fr/tracker/bug_report_page.php" class="leaving-link" onclick="window.open(this.href,'openvibe-tracker'); return false;">bug report</a>
		when you find bugs.</p>

		<h2>Website launched<br><span style="font-size:x-small">2009/05/05</span></h2>

		<p>The website of the software is operational... We hope you'll find all needed information here.</p>
		<p>Long live OpenViBE !</p>

		<h2>Training session<br><span style="font-size:x-small">2009/01/15</span></h2>

		<p>A training session took place at INRIA, Rennes, France. The objective of this training session was to
		present the software and its capabilities, as well as to provide hands-on training. In addition to an
		overview of the main features of OpenViBE, the session included information targeted at developers. After core
		OpenViBE concepts and the software architecture were presented, tutorials and exercises offered trainees the
		opportunity to try their hand at modifying and/or extending the OpenViBE software (e.g. by writing new
		acquisition drivers and new signal processing boxes).</p>
		<p>15 people attended this training session.</p>
		<p>You can download <a href="pub/doc/training/2009-01-14-training-course.pdf">the slides</a> and the <a href="pub/src/exercices">exercises</a>.</p>
	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="about">

		<h1>About OpenViBE</h1>

		<p>OpenViBE is a software platform dedicated to designing, testing and using <a href="?q=bci">brain-computer interfaces</a>.</p>
		<p>OpenViBE is a software for real-time neurosciences (that is, for real-time processing of brain signals).
		It can be used to acquire, filter, process, classify and visualize brain signals in real time.</p>

		<h1>Applications</h1>

		<p>The main OpenViBE application fields are medical (assistance to disabled people, real-time biofeedback,
		neurofeedback, real-time diagnosis), multimedia (virtual reality, video games), robotics and all other
		application fields related to brain-computer interfaces and real-time neurosciences.</p>
		<p>OpenViBE users can either be programmers or people not familiar with programming. This includes medical
		doctors, video game developers, researchers in signal-processing or robotics, etc.</p>

		<h1>OpenViBE Characteristics</h1>

		<p>The graphical user interface of OpenViBE is simple to access and very easy to use for creating BCI
		scenarios and saving them for later use. In the designer, the available functions are listed in the
		right-hand window. The user simply drags and drops the selected functions in the left-hand window.
		He can then connect boxes together to add processing steps to the scenario being created.
		Lastly, the application is started by pressing the <i>Play</i> button to run the BCI.</p>
		<p>OpenViBE is a library of functions written in C++ which can be integrated and applied quickly and
		easily using modules. The platform's main advantages are modularity, portability, availability of
		different tools for different types of user, including programmers and non-programmers, superior
		code performance and compatibility with virtual reality technologies.</p>
		<p>The software also offers many 2D and 3D visualization tools to represent brain activity in real time.
		It is compatible with many EEG- and MEG-type machines because of its generic acquisition server.</p>
		<p>OpenViBE offers many pre-configured scenarios for different applications including mental imagery,
		neurofeedback, P300 signals, etc.</p>

		<h1>OpenViBE Status</h1>

		<p>OpenViBE is open source and available for free on the INRIA forge.</p>
		<p>Don't hesitate to download OpenViBE and <a href="?q=download">give it a try.</a></p>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="features">

		<h1>Key Features</h1>

		<p>OpenViBE enables to design, test and use <a href="?q=bci">Brain-Computer Interfaces</a>.</p>
		<p>OpenViBE is free and open source (under the terms of the <a href="documentation/unstable/Doc_Licence.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">LGPL-v2+</a>).
		The whole software is developed in C++. It consists of a set of software modules that can be integrated easily and efficiently to design
		BCI applications such as for Virtual Reality (VR) interaction. Key features of the platform are:
		<ul>
			<li><b>Modularity and reusability.</b> Our platform is a set of software modules dedicated to the acquisition, pre-processing,
			processing and visualization of cerebral data, as well as to the interaction with VR displays. OpenViBE is a general
			purpose software, which implies users should be able to easily add new software modules in order to fit their particular needs. This is
			ensured thanks to the concept of box algorithm, a "black box" in charge of a fraction of the whole processing pipeline, that
			allows to develop reusable components, reduces development time and helps to quickly extend functionalities<!-- (See Box algorithms list for a list of existing box algorithms)-->.</li>
			<li><b>Different user types.</b> OpenViBE is designed for different types of users: VR developers, clinicians, BCI researchers,
			etc. Their various needs are addressed and different tools are proposed for each of them, depending on their programming
			skills and their knowledge in brain processes.</li>
    		<li><b>Portability.</b> The platform operates independently from the different software and hardware targets. It includes
			an abstraction allowing to run with various acquisition machines, such as EEG or MEG. It can run on Windows and Linux and
			also includes different data visualisation techniques. Also, it is based on free and portable softwares (e.g., GTK+, IT++,
			GSL, VRPN, GCC).</li>
    		<li><b>Connection with VR.</b> Our software can be integrated with high-end VR applications. OpenViBE can serve as an
			external peripheral to connect a BCI system to any kind of VR application. It also takes advantage of VR displays thanks
			to a light abstraction of a scenegraph management library, allowing to visualize cerebral activity more efficiently or to
			provide incentive training environments (e.g., for neurofeedback).</li>
		</ul>
		</p>
		<p>As compared to other BCI softwares, the OpenViBE platform stands out for its high modularity. It addresses the needs
		of different types of users (programmers and non-programmers) and proposes a user-friendly graphical language which allows
		non-programmers to design a BCI without writing a single line of code. Moreover, OpenViBE is portable, independent of
		hardware or software targets, can run under Windows and Linux and is entirely based on free and open-source software.</p>

		<h1>Existing scenarios</h1>

		<p>You can use preconfigured scenarios and run already existing applications :</p>
		<ul>
			<li>1-class BCI based on motor imagery</li>
			<li>2-class BCI based on motor imagery</li>
			<li>Neurofeedback</li>
			<li>Real-time visualization of brain activity in 2D or 3D</li>
			<li>P300 speller</li>
		</ul>

		<h1>Detailed list of functionalities</h1>

		<p>OpenViBE includes the following functionalities (<i>automatically extracted from the
		<a href="documentation/unstable/Doc_FAQ.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">frequently asked questions</a> page</i>) :</p>

		<script language=javascript type='text/javascript'>
			clientSideInclude("documentation/unstable/Doc_FAQ.html", "<!-- features start", "features stop -->");
		</script>

		<h1>Supported acquisition devices</h1>

		<p>OpenViBE includes drivers for the following devices (<i>automatically extracted from the
		<a href="documentation/unstable/Doc_FAQ.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">frequently asked questions</a> page</i>) :</p>

		<script language=javascript type='text/javascript'>
			clientSideInclude("documentation/unstable/Doc_FAQ.html", "<!-- hardware-compatibility start", "hardware-compatibility stop -->");
		</script>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="made-with">

		<h1>Made With OpenViBE !</h1>

		<p>This section shows what our users are doing with OpenViBE.</p>
		<p>Do you want to be listed here ? We are interested in your feedback. Feel free to post a short presentation on the
		<a href=http://openvibe.inria.fr/forum/viewtopic.php?t=270>dedicated forum topic</a>.</p>

		<h2>2009/01/28 Robotic arm controlled by OpenViBE<br><span style="font-size:x-small">Reported by Dieter Devlaminck</span></h2>

		Dieter Devlaminck, researcher at <a href=http://www.ugent.be/en>Ghent University</a>, reported his recent results with OpenViBE : controlling a robotic arm
		thanks to P300 signals. What he did with OpenViBE was basically to derive the xDAWN based P300 signals speller scenario. Each letter in the grid allows to
		send a specific command to the robot. He posted a video demonstrating those results.

		<div style="padding:1em; margin-top: 1em; margin-bottom: 1em; margin-left: 4em; margin-right: 4em; border: 1px solid #D0D0D0; background-color: #F0F0F0; text-align: center; -moz-border-radius: 0.5em"><object width="400" height="300"><param name="allowfullscreen" value="true" /><param name="allowscriptaccess" value="always" /><param name="movie" value="http://vimeo.com/moogaloop.swf?clip_id=8841995&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;show_portrait=0&amp;color=&amp;fullscreen=1" /><embed src="http://vimeo.com/moogaloop.swf?clip_id=8841995&amp;server=vimeo.com&amp;show_title=1&amp;show_byline=1&amp;show_portrait=0&amp;color=&amp;fullscreen=1" type="application/x-shockwave-flash" allowfullscreen="true" allowscriptaccess="always" width="400" height="300"></embed></object><p><a href="http://vimeo.com/8841995">Robotic toy arm controlled through brain-computer interface</a> from <a href="http://vimeo.com/user2212371">Dieter Devlaminck</a> on <a href="http://vimeo.com">Vimeo</a>.</p></div>

		Warm thanks to Dieter Devlaminck for reporting this work to us !

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="jobs">

		<h1>Job Offers</h1>

<!-- -->
		<p>There are several opened positinos to work on and with OpenViBE...</p>
<!--
		<p>There is currently no opened positions to work on and with OpenViBE...</p>
-->
<!--
		<script language=javascript type='text/javascript'>
			document.write(available_soon('#FIXME'));
		</script>
-->
		<h2>One post-doc position at Hopital Raymond Poincare<br><span style="font-size:x-small">2010-12</span></h2>

		<p><u><b>Short Description:</b></u></p>
		<p>The Raymond Poincaré Hospital in Paris (France) has an open position for a Post-Doc to support clinical trials in Brain-Computer Interfaces, as described in the enclosed document.</p>
		<br>
		<p>You can download the full <a href=jobs/2010-12-APHP-Post-Doc.pdf>english PDF description</a>.</p>
		<br>
		<p>Interested candidates should contact Louis Mayaud (<i>louis dot mayaud at rpc dot aphp dot fr</i>)</p>
		<h2>Three open positions at INRIA Sophia Antipolis<br><span style="font-size:x-small">2010-12</span></h2>

		<p><u><b>Short Description:</b></u></p>
		<p>The Athena Team is looking after one experienced engineer, one phd and one post-doc in the particular field of Brain-Computer Interfaces.</p>
		<br>
		<p>You will find more information on the dedicated pages:</p>
		<ul>
		<li><a href=http://www-sop.inria.fr/athena/Site/CoAdaptEngineer>Experienced Engineer</a> position descriptions</li>
		<li><a href=http://www-sop.inria.fr/athena/Site/CoAdaptPhD>Internship + PhD</a> position descriptions</li>
		<li><a href=http://www-sop.inria.fr/athena/Site/CoAdaptPostdoc>Post-doc</a> position descriptions</li>
		<br>
		<p>Interested candidates should contact Maureen Clerc (<i>maureen dot clerc at inria dot fr</i>)</p>

		<h2>One software engineer position at Hopital Raymond Poincare<br><span style="font-size:x-small">2010-06 - <span style="text-color:red">CLOSED</span></span></h2>

		<p><u><b>Short Description:</b></u></p>
		<p>Robust Brain Computer Interface virtual Keyboard (RoBIK)</p>
		<p>The principle of a Brain-Computer Interface or BCI is to control a device through the extraction and interpretation of signal features from electroencephalographic (EEG) collected on the surface of the scalp or by the mean of invasive measurements. This old idea of communication technique (Vidal 1973), offers the advantage to bypass need of muscle activity in the control chain and therefore is naturally presented as a promising alternative for restoration of control and communication of people with neuromuscular disabilities (Wolpow, et al. 2002).</p>
		<p>However BCI technology remains an object of study for research laboratories, it is not spread out in patients¿ daily life. We have identified the lack of robustness and bad ergonomics of current BCI to be the main reasons of this situation. Therefore, despite the large number of research around this topic today, there is little offer of BCI system for disabled users. The project aims at developing a patient-dedicated application using the open-source project called OpenViBE.</p>
		<br>
		<p>You can download the full <a href=jobs/2010-06-HRP-Garches-Software-Engineer-en.pdf>english PDF description</a>.</p>
		<br>
		<p>Interested candidates should contact Louis Mayaud (<i>louis dot mayaud at rpc dot aphp dot fr</i>)</p>

		<h2>One post-doc position at INRIA<br><span style="font-size:x-small">2010-01 - <span style="text-color:red">CLOSED</span></span></h2>

		<b>
		<p><u>Post-doctoral position on:</u></p>
		<p>"The use of Brain-Computer Interfaces for 3D Interaction with Videogames and Virtual Environments"</p>
		</b>

		<p>2-year post-doctoral position is available at INRIA (National Institute of Research in Computer Science and Control), Rennes, France.</p>
		<p>The project is in the area of <i>virtual reality</i> (VR) and <i>Brain-Computer Interfaces</i> (BCI). It concerns the use of BCI for the purpose of 3D interaction with virtual worlds and videogames. The postdoc is strongly related to OpenViBE software for BCI and VR (<a href=http://openvibe.inria.fr>http://openvibe.inria.fr</a>) and to OpenViBE2 research project, which targets "future BCI usages for videogames". The postdoctoral fellow will work within a research team devoted to BCI and VR and under the supervision of Dr. Anatole L&eacute;cuyer (<a href=http://www.irisa.fr/bunraku/anatole.lecuyer>http://www.irisa.fr/bunraku/anatole.lecuyer</a>).</p>
		<p>The post-doctoral stay is funded for 2 years. The post-doctoral program aims at designing and testing novel 3D interaction techniques with virtual worlds based on Brain-Computer Interfaces. Brain-computer Interfaces (BCI) enable a user to send commands to a computer using only variations of his brain activity. Traditionally, BCI aim at providing communication to those who have lost their voluntary muscle control. They can also be used for multimedia applications, such as for the purpose of interacting with videogames and 3D virtual worlds.</p>
		<p>Main objective of the post-doctoral work is to investigate and design novel interaction techniques specifically adapted to the use of a brain-computer interfaces for 3D virtual environments. These BCI-based techniques are expected to improve 3D tasks such as navigation in the virtual world or selection and manipulation of virtual objects "by thought". A series of experiments with participants will be conducted to evaluate the various proposed techniques.</p>
		<p>The candidate must have a PhD and an excellent background in either: virtual reality, 3D user interfaces, brain-computer interaction, human-computer interfaces, or other relevant topics.</p>
		<p>Interested candidates should send CV, selected publications and names and addresses of three references to :</p>
		<br>
		<p>Dr. Anatole L&eacute;cuyer, INRIA Rennes (<i>anatole dot lecuyer at irisa dot fr</i>)</p>
		<br>
		<p>References</p>
		<ul>
			<li>F. Lotte, A. Van Langhenhove, F. Lamarche, T. Ernest, Y. Renard, B. Arnaldi, A. L&eacute;cuyer, "Exploring Large Virtual Environments by Thoughts using a Brain-Computer Interface based on Motor Imagery and High-Level Commands", Presence : teleoperators and virtual environments, vol. 19, no 1, 2010 (in press)</li>
			<li>Y. Renard, F. Lotte, G. Gibert, M. Congedo, E. Maby, V. Delannoy, O. Bertrand, A. L&eacute;cuyer, "OpenViBE: An Open-Source Software Platform to Design, Test and Use Brain-Computer Interfaces in Real and Virtual Environments", Presence : teleoperators and virtual environments, vol. 19, no 1, 2010 (in press)</li>
			<li>J.B. Sauvan, A. L&eacute;cuyer, F. Lotte, G. Casiez, "A Performance Model of Selection Techniques for P300-Based Brain-Computer Interfaces", ACM SIGCHI Conference on Human Factors in Computing Systems (ACM CHI), 2009</li>
			<li>F. Lotte, J. Fujisawa, H. Touyama, R. Ito, M. Hirose, A. L&eacute;cuyer, "Towards Ambulatory Brain-Computer Interfaces: A Pilot Study with P300 Signals", Advances in Computer Entertainment Technology Conference (ACE), 2009</li>
			<li>A. L&eacute;cuyer, F. Lotte, R. Reilly, R. Leeb, M. Hirose, M. Slater, "Brain-Computer Interfaces, Virtual Reality, and Videogames", IEEE Computer, vol. 41, num 10, pp. 66-72, 2008</li>
		</ul>

		<h2>One software engineer (CEA/Leti, Grenoble, France) for a 18-months fixed period contract<br><span style="font-size:x-small">2010-03 - <span style="text-color:red">CLOSED</span></span></h2>

		<p><u><b>Short Description:</b></u></p>
		<p>The overall goal of the BCI project under the scientific direction of Pr BENABID is to develop an implantable device to allow tetraplaegic patients pilot n degrees of freedom effectors (wheelchair, robotized arm, exosqueletton).</p>
		<p>The software engineer will work within a team of electronics, software and signal processing engineers, researchers and biologists. He will be in charge to implement OpenViBE as an experimental framework for advanced BCI projects, with another software engineer. The framework must be able to:</p>
		<ul>
			<li>Acquire data from CEA/LETI devices during preclinical studies</li>
			<li>Perform some data processing</li>
			<li>Control some effectors</li>
		</ul>
		<p>Integrated to the Clinatec team, the engineer will be mainly in charge of designing new OpenViBE boxes. Based on biologists and neurosurgeons requests he will have to deliver them with new boxes they will need during an innovative BCI project. Boxes will mainly do very basic data processing and control effectors. Possibly, you may also be asked to design standalone C++/Qt applications.</p>
		<p>Main missions will be to implement and extend the functionalities of the OpenViBE-based framework (C++ programming, object oriented design) and ensure its support and deployment. Skills in GTK/Qt are appreciated.</p>
		<p>Feel free to contact us for more information: <i>jean [dot] porcherot [at] cea [dot] fr</i></p>
		<p>You can download the <a href=jobs/2010-03-CEA-Leti-Grenoble-Software-Engineer.pdf>french PDF description</a>.</p>

		<h2>One engineer and two post-doc positions at Gipsa-lab<br><span style="font-size:x-small">2009-06 - <span style="text-color:red">CLOSED</span></span></h2>

		<p>Two opened positions in the framework of the GAZE &amp; EEG (Gipsa-lab, Grenoble, France) :</p>
		<ul>
			<li>One Software or Electrical Engineer, PDF : <a href="jobs/2009-06-GipsaLab-Grenoble-GAZE_EEG-SoftwareEngineer_PostDoc-en.pdf">english</a></li>
			<li>One Post-doc in EEG signal processing, PDF : <a href="jobs/2009-06-GipsaLab-Grenoble-GAZE_EEG-SoftwareEngineer_PostDoc-en.pdf">english</a></li>
		</ul>
		<p>One opened position in EEG signal processing/machine learning for Brain-Computer Interface and Video Gaming (Gipsa-lab, Grenoble, France) :</p>
		<ul>
			<li>One Post-doc in EEG signal processing/machine learning, PDF : <a href="jobs/2009-06-GipsaLab-Grenoble-OpenViBE2-PostDoc-en.pdf">english</a></li>
		</ul>
		<p>Feel free to contact us for more information : marco [dot] congedo [at] gmail [dot] com</p>

		<h2>Two software engineer positions at INRIA<br><span style="font-size:x-small">2009-06 - <span style="text-color:red">CLOSED</span></span></h2>

		<p>One software engineer (INRIA, Nancy, France)</p>
		<p>One software engineer (INRIA, Rennes, France)</p>
		<p>Short Description:</p>
		<p>The software engineer will work within a team of engineers, researchers, students working
		on the OpenViBE software and on Brain-Computer Interfaces, in collaboration with other INRIA
		Centers (Rennes, Nancy, Sophia), and other partners (<b>INSERM</b>, <b>CEA</b>, <b>UBISOFT</b>) ; notably in the
		frame of a collaborative project <b><em>OpenViBE2</em></b> involving 10 partners and focusing on the use of
		Brain-Computer Interfaces for video games.</p>
		<p>The software engineer will work on development and scientific research related to OpenViBE
		software. Main missions will be to extend the functionalities of the OpenViBE software
		(C++ programming) and ensure its support and deployment. We hope OpenViBE will become a
		standard software in the scientific communities working on BCI. For this aim, the main
		tasks of the successful will consist in:</p>
		<ul>
			<li>develop novel functionalities of the software to match users needs (for instance:
			increase of computational capabilities, advanced 3D brain visualization, compatibility
			with other software and machines)</li>
			<li>ensure support, maintenance and spreading of the software (forge, website, forum,
			training session, etc).</li>
		</ul>
		<p>More information:</p>
		<ul>
			<li>Software engineer (INRIA, Nancy, France), PDF : <a href="jobs/2009-06-INRIA-Nancy-Software-Engineer-fr.pdf">french</a>/<a href="jobs/2009-06-INRIA-Nancy-Software-Engineer-en.pdf">english</a></li>
			<li>Software engineer (INRIA, Rennes, France), PDF : <a href="jobs/2009-06-INRIA-Rennes-Software-Engineer-fr.pdf">french</a>/<a href="jobs/2009-06-INRIA-Rennes-Software-Engineer-en.pdf">english</a></li>
		</ul>
		<p>Feel free to contact us for more information : anatole [dot] lecuyer [at] irisa [dot] fr</p>

		<h2>One PhD Thesis position at INRIA<br><span style="font-size:x-small">2009-06 - <span style="text-color:red">CLOSED</span></span></h2>

		<p>One PhD Thesis on "3D interaction with video games based on Brain-Computer Interfaces"
		(INRIA, Rennes, France)</p>
		<p>Short Description:</p>
		<p>The objective of this PhD Thesis is to study the use of Brain-Computer Interfaces to
		interact with video games. Brain-Computer Interfaces (or BCI) correspond to the direct
		use of brain signals to send "mental commands" to an automated system such as a robot,
		a prosthesis, or a cursor on a computer screen. Cerebral activity is measured through
		ElectroEncephaloGraphy (EEG), that is with electrodes located at the surface of the scalp
		and recording electrical signals emitted by the brain.</p>
		<p>This PhD Thesis is in the frame of a 3-year collaborative project called <b><em>OpenViBE2</em></b>
		(2009-2012) involving 10 French partners (such as <b>INRIA</b>, <b>INSERM</b>, <b>UBISOFT</b>
		and <b>CEA</b>) and focusing on the use of Brain-Computer Interfaces for video games. It is
		also closely related to the development of the OpenViBE software (http://openvibe.inria.fr) -
		a free and open-source software platform dedicated to designing, testing and using
		brain-computer interfaces.</p>
		<p>Brain-Computer Interfaces are no longer a dream today as several impressive prototypes
		have been developed in recent years. However, few laboratories have studied the integration
		of BCI with Virtual Reality technologies and with video games [L&eacute;cuyer et al., 2008]. In
		this PhD Thesis, the aim will be to design, develop and test new methods to improve the
		use of BCI to interact with virtual worlds. Novel interaction techniques will be proposed
		that concern one or several classical tasks operated in virtual environments such as:
		navigation in virtual world, selection and grasping of virtual objects, manipulation of
		virtual objects, or application control (menus, buttons, etc) - all these elementary tasks
		being achieved "by thought". Experimentations are planned to evaluate and validate the
		developments and the approach using EEG acquisition machine.</p>

		<ul>
			<li>PhD Thesis (INRIA, Rennes, France), PDF : <a href="jobs/2009-06-INRIA-Rennes-PhD-Thesis-fr.pdf">french</a>/<a href="jobs/2009-06-INRIA-Rennes-PhD-Thesis-en.pdf">english</a></li>
		</ul>
		<p>Feel free to contact us for more information : anatole [dot] lecuyer [at] irisa [dot] fr</p>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="download">

		<h1>Installer Download</h1>

		<h2>Windows (32 bits)</h2>

		<script language=javascript type='text/javascript'>
			precompiled("OpenViBE 0.12.0", "pub/bin/win32/openvibe-0.12.0-svn3107-setup", "2011/10/12", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
		</script>
		<div id="win32-installer-download-n" class="old-download">
			<script language=javascript type='text/javascript'>
				precompiled("OpenViBE 0.11.0", "pub/bin/win32/openvibe-0.11.0-svn3052-setup", "2011/08/11", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
				//precompiled("OpenViBE 0.10.1", "pub/bin/win32/openvibe-0.10.1-svn2909-setup", "2011/04/11", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
				//precompiled("OpenViBE 0.9.0", "pub/bin/win32/openvibe-0.9.0-svn2639-setup", "2010/12/28", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
				//precompiled("OpenViBE 0.8.0", "pub/bin/win32/openvibe-0.8.0-svn2427-setup", "2010/10/01", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
				// precompiled("OpenViBE 0.7.0", "pub/bin/win32/openvibe-0.7.0-svn2254-setup", "2010/07/08", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
				// precompiled("OpenViBE 0.6.1", "pub/bin/win32/openvibe-0.6.1-svn2075-setup", "2010/05/20", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...<br><br><br><br><br>This is a bug fix release :<br><ul><li>Restores the TMSi driver</li><li>Restores the Micromed driver</li><li>Translates the Tie-Fighter demo in english</li></ul>", "openvibe-precompiled.png", 190);
				// precompiled("OpenViBE 0.6.0", "pub/bin/win32/openvibe-0.6.0-svn2044-setup", "2010/04/14", "Requires Windows 2000/XP/Vista/7<br>Includes the Acquisition Server, the Designer, the VR demos, some sample scenarios and data files...", "openvibe-precompiled.png");
				// precompiled("OpenViBE 0.5.0", "pub/bin/win32/openvibe-0.5.0-svn1827-setup", "2009/12/18", "Requires Windows 2000/XP/Vista<br>Includes the Acquisition Server, the Designer, some sample scenarios and data files...", "openvibe-precompiled.png");
				// precompiled("OpenViBE 0.4.0", "pub/bin/win32/openvibe-0.4.0-svn1737-setup", "2009/10/23", "Requires Windows 2000/XP/Vista<br>Includes the Acquisition Server, the Designer, some sample scenarios and data files...", "openvibe-precompiled.png");
				// precompiled("OpenViBE 0.4.0 RC 4", "pub/bin/win32/openvibe-0.4.0-rc4-svn1651-setup", "2009/06/29", "Requires Windows 2000/XP/Vista<br>Includes the Acquisition Server, the Designer, some sample scenarios and data files...", "openvibe-precompiled.png");
				// precompiled("OpenViBE 0.4.0 RC 3", "pub/bin/win32/openvibe-0.4.0-rc3-setup", "2009/05/05", "Requires Windows 2000/XP/Vista<br>Includes the Acquisition Server, the Designer, some sample scenarios and data files...", "openvibe-precompiled.png");
			</script>
			<div class="older-installer"><p><img src="openvibe-precompiled-older.png" class="screenshot">Want even older versions ?</p><p>Browse the <a href="pub/bin/win32" onclick="window.open(this.href,'openvibe-file-browser'); return false;">archives</a>...</p></div>
		</div>
		<div id="win32-installer-download-y" class="old-download">
			<div class="older-installer"><p><img src="openvibe-precompiled-older.png" class="screenshot">Want <a href="javascript:remove_div('win32-installer-download-y'); javascript:show_div('win32-installer-download-n');">older versions</a> ?</p></div>
		</div>

		<h2>Windows (64 bits)</h2>

		<p>OpenViBE has been reported to compile and run correctly on 64 bits Windows. However, we do not provide auto installers yet. If you want to contribute and have skills in building such an installer for 64 bits Windows, feel free to contact us.</p>

		<h2>Linux (32 & 64 bits)</h2>

		<p>For now, Linux precompiled packages are missing. If you want to contribute and have skills in building <i>RPMs</i>, <i>DEBs</i> or whatever package your distro uses, feel free to contact us.</p>

		<h1>Source code download</h1>

		<p>If you can't find an installer for your platform, you can download and build the software from its sources.</p>(<i>automatically extracted from the
		<a href="documentation/unstable/Doc_FAQ.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">frequently asked questions</a> page</i>) :</p>

		<script language=javascript type='text/javascript'>
			clientSideInclude("documentation/unstable/Doc_FAQ.html", "<!-- os-compatibility start", "os-compatibility stop -->");
		</script>

		<p>Check the <a href="documentation/unstable/Doc_Install.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">installation instructions</a> for the procedure to follow. If you have any problem building the
		software, feel free to <a href="?q=support">request help</a></p>

		<h2>Archives</h2>

		<p>The software is available under the terms of the <a href="documentation/unstable/Doc_Licence.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">LGPL-v2+</a>
		with the exception of a couple of plugins that rely on GPL libraries and become GPL themselves.</p>
		<p>Stable code is distributed as compressed source archives :</p>

		<script language=javascript type='text/javascript'>
			archive("OpenViBE 0.11.0 (svn 3052)", "pub/src/openvibe-0.11.0-svn3052-src", "2011/08/11", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
		</script>
		<div id="src-download-n" class="old-download">
			<script language=javascript type='text/javascript'>
				archive("OpenViBE 0.10.0 (svn 2909)", "pub/src/openvibe-0.10.0-svn2909-src", "2011/04/11", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.9.0 (svn 2639)", "pub/src/openvibe-0.9.0-svn2639-src", "2010/12/28", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.8.0 (svn 2427)", "pub/src/openvibe-0.8.0-svn2427-src", "2010/10/01", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.7.0 (svn 2254)", "pub/src/openvibe-0.7.0-svn2254-src", "2010/07/08", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.6.0 (svn 2044)", "pub/src/openvibe-0.6.0-svn2044-src", "2010/04/14", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.5.0 (svn 1827)", "pub/src/openvibe-0.5.0-svn1827-src", "2009/12/18", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.4.0 (svn 1737)", "pub/src/openvibe-0.4.0-svn1737-src", "2009/10/23", "Check the <a href=\"http://openvibe.inria.fr/documentation/unstable/Doc_Install.html\" class=\"leaving-link\" onclick=\"window.open(this.href,'openvibe-doc'); return false;\">Installation Procedure</a> to build the software.", "openvibe-sources.png");
				// archive("OpenViBE 0.4.0 RC 4 (svn 1651)", "pub/src/openvibe-0.4.0-rc4-svn1651-src", "2009/06/29", "", "openvibe-sources.png");
				// archive("OpenViBE 0.3.0", "pub/src/openvibe-0.3.0-src", "2007/10/17", "", "openvibe-sources.png");
			</script>
			<div class="older-archive"><p><img src="openvibe-sources-older.png" class="screenshot">Want even older versions ?</p><p>Browse the <a href="pub/src" onclick="window.open(this.href,'openvibe-file-browser'); return false;">archives</a>...</p></div>
		</div>
		<div id="src-download-y" class="old-download">
			<div class="older-archive"><p><img src="openvibe-sources-older.png" class="screenshot">Want <a href="javascript:remove_div('src-download-y'); javascript:show_div('src-download-n');">older versions</a> ?</p></div>
		</div>

		<h2>Subversion</h2>

		<p>The software is available under the terms of the <a href="documentation/unstable/Doc_Licence.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">LGPL-v2+</a>
		with the exception of a couple of plugins that rely on GPL libraries and become GPL themselves.</p>
		<p>The most recent code may be retrieved anonymously from the <a href="http://gforge.inria.fr/projects/openvibe" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">INRIA gForge</a> using <i>subversion</i> :</p>
		<p><div class="code">svn checkout svn://scm.gforge.inria.fr/svn/openvibe/trunk</div></p>
		<p>Alternatively, if you plan to directly contribute into the INRIA gForge and already have an account, you should use <i>subversion</i> over <i>ssh</i> :</p>
		<p><div class="code">svn checkout svn+ssh://<b>accountname</b>@scm.gforge.inria.fr/svn/openvibe/trunk</div></p>

<!--
		<h1>Virtual Machine</h1>

		<p><span style="font-weight: bold; color: red">WARNING:</span> The virtual machine is outdated ! We will update is as soon as possible.</p>

		<p>In order to quickly evaluate OpenViBE on Linux, we propose preconfigured <a href="www.virtualbox.org" class="leaving-link">VirtualBox</a> virtual machines.
		<span style="font-weight: bold; color: red">Please note that this is for quick evaluation only</span>. Do not expect the full horsepower that you'd get
		compiling and executing the software natively on your computer.</p>
		<p>In order to install and use this virtual machine, you will have to :</p>
		<ul>
			<li>if you never started virtual box, start it once so it creates its configuration files</li>
			<li>shutdown virtual box</li>
			<li>download the <b>7z</b> hard disk image</li>
			<li>uncompress the <b>7z</b> hard disk image</li>
			<li>move the <em>.vdi</em> file in your <em>~/.VirtualBox/VDI</em> folder (you can create it if it does not exist)</li>
			<li>download the <em>.xml</em> virtual machine configuration</li>
			<li>move the <em>.xml</em> file in your <em>~/.VirtualBox/Machines</em> folder (you can create it if it does not exist)</li>
			<li>add this line :<em>&lt;MachineEntry src="Machines/<b>the-filename.xml</b>" uuid="{39f83d91-eb63-403f-539f-e6a0bbbb826e}" /&gt;</em> to the <em>MachineRegistry</em> section of your <em>~/.VirtualBox/VirtualBox.xml</em> configuration file</li>
			<li>add this line :<em>&lt;HardDisk type="normal" uuid="{0c741db0-2794-4785-fe9c-d098d3e44513}"&gt;&lt;VirtualDiskImage filePath="VDI/openvibe.vdi"/&gt;&lt;/HardDisk&gt;</em> to the <em>DiskRegistry</em> section of your <em>~/.VirtualBox/VirtualBox.xml</em> configuration file</li>
			<li>optionnaly, you may put the last line betwenn <em>&lt;HardDisks&gt;</em> and <em>&lt;/HardDisks&gt;</em> tags if they do not exist</li>
			<li>start VirtualBox</li>
			<li>start the virtual machine</li>
		</ul>
		<p><span style="font-weight: bold; color: red">Latest versions of <a href="www.virtualbox.org" class="leaving-link">VirtualBox</a> have import/export feature</span>.

		<script language=javascript type='text/javascript'>
			virtualmachine("OpenViBE 0.4.0 (svn 1737)", "pub/vm/openvibe-0.4.0-svn1737-vm", "2009/10/23", "This virtual machine is based on minimal install of <a href=http://www.xubuntu.org class=leaving-link>xubuntu 9.04 i386</a>. "+
			               "OpenViBE dependencies have been installed from the distro repository. OpenViBE has been checkedout and compiled directly on the virtual machine. "+
			               "In case you need it, the login name is <em>openvibe</em> and the password is <em>openvibe</em> (but autologin is activated so you don't have "+
			               "to enter this (until you actually logout or require root access). The virtual machine has been generated on ubuntu 9.04 with Virtual Box 2.1.4_OSE. "+
			               "", "openvibe-virtualmachine.png");
/*
			virtualmachine("OpenViBE 0.4.0 RC 4 (svn 1651)", "pub/vm/openvibe-0.4.0-rc4-svn1651-vm", "2009/06/29", "This virtual machine is based on minimal install of <a href=http://www.xubuntu.org class=leaving-link>xubuntu 9.04 i386</a>. "+
			               "OpenViBE dependencies have been installed from the distro repository. OpenViBE has been checkedout and compiled directly on the virtual machine. "+
			               "In case you need it, the login name is <em>openvibe</em> and the password is <em>openvibe</em> (but autologin is activated so you don't have "+
			               "to enter this (until you actually logout or require root access). The virtual machine has been generated on ubuntu 8.04.2 with Virtual Box 1.5.6_OSE. "+
			               "", "openvibe-virtualmachine.png");
*/
		</script>
-->

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="support">

		<h1>Support</h1>

		<p>To begin with, you can watch our introductory <a href="?q=medias">videos</a>. The first is a <a href="#FIXME">brief overview</a> of OpenViBE and BCI.
		The second is an extensive 1-hour tour of the platform in the form of a <a href="video.php?q=video-designer-training-en&playlist=true">training session</a> on how to use the software.</p>
		<p>You can visit the <a href="documentation/unstable" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">OpenViBE documentation</a> website.
		In addition to a number of tutorials, tips and usage recommendations to better use the software, there is <a href="documentation/unstable/Doc_UserDocumentation.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">user documentation</a>
		both for the <a href="documentation/unstable/Doc_DesignerUserDocumentation.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">designer</a> and the
		<a href="documentation/unstable/Doc_AcquisitionServerOverview.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">acquisition server</a>. Newcomers
		should probably check the <a href="documentation/unstable/Doc_Install.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">installation procedure</a>
		and the <a href="documentation/unstable/Doc_FAQ.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">frequently asked questions</a>.
		Developers should head the <a href="documentation/unstable/Doc_DeveloperDocumentation.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">developer documentation</a>
		to find tutorials on how to create a new <a href="documentation/unstable/Doc_CreatingANewDriver.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">acquisition driver</a>
		or a new <a href="documentation/unstable/Doc_Tutorial_Developer_SignalProcessing.html" class="leaving-link" onclick="window.open(this.href,'openvibe-doc'); return false;">box</a>.
		<p>If you need further support, the <a href="http://openvibe.inria.fr/forum" class="leaving-link" onclick="window.open(this.href,'openvibe-forum'); return false;">forum</a> is a good place to start with.</p>
		<p>Additionally, you can use the <a href="https://gforge.inria.fr/mail/?group_id=276" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">mailing lists</a>
		or <b>IRC</b> to contact us (<i>#openvibe</i> on <i>irc.freenode.net</i>).</p>
		<p>If you find bugs, please take the time to fill an entry in the <a href="http://openvibe.inria.fr/tracker/bug_report_page.php" class="leaving-link" onclick="window.open(this.href,'openvibe-tracker'); return false;">bug tracker</a>.</p>
		<p>We hope you'll enjoy working with OpenViBE!</p>
	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="bci">

		<h1>What is a Brain Computer Interface ?</h1>

		<p>Brain-Computer Interaction (or BCI) corresponds to the direct use of brain signals to send "mental commands" to
		an automated system such as a robot, a prosthesis, or a cursor on a computer screen. BCI is a rapidly growing area
		of research and several impressive prototypes are already available. Indeed, severely disabled people are
		enabled to control the motion of their wheelchair or to communicate with their relatives using adapted mental "spellers"!</p>

		<h1>Applications</h1>

		<p>Typical BCI applications belong the the following fields : medical (assistance to disabled people, real-time biofeedback,
		real-time diagnosis), multimedia (virtual reality, video games), robotics (control of robots, prosthesis, etc), domotics
		or home automation, etc.</p>

		<h1>Examples</h1>

		<h2>Use-the-force : Are you a Jedi ?</h2>

		<script language=javascript type='text/javascript'>
			screenshot("openvibe-snapshot-vr-app-01.png", "The <b>Use the force</b> application is an entertaining, easy to use Virtual Reality (VR) game designed to enable naive "+
			                                              "subjects to interact with a simple virtual world. It is inspired by the <i>Star Wars</i> movie and displays a spaceship docked "+
			                                              "in a space station. This self-paced game lets users lift the spaceship in the air at any time during the experiment. "+
			                                              "This is achieved by way of motor imagery : after an initial baseline-measuring phase, users must perform real or imagined "+
			                                              "foot movements which, when stopped, are followed by a synchronisation in the beta band detected by our system. The spaceship "+
			                                              "is then temporarily lifted at a speed proportional to the level of the synchronisation.");
		</script>

		<h2>Virtual HandBall</h2>

		<script language=javascript type='text/javascript'>
			screenshot("openvibe-snapshot-vr-app-02.png", "The <b>Handball</b> application is an entertaining game based on a 2 class motor imagery protocol. In this application, "+
			                                              "subjects must control a ball to score goals in a handball playing court. Two goals are located on each side of the "+
			                                              "court, and a ball is placed in between them (in the center of the screen) at the start of each trial. During control "+
			                                              "phases, the game displays cues on screen to tell users when and where they should move the ball to score. The protocol "+
			                                              "used by this application is similar to that used in the Graz motor imagery BCI : users have to perform imagined "+
			                                              "left or right hand movements, as instructed by cues (left and right arrows) displayed on screen.");
		</script>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="medias">

		<a name="medias-videos" />
		<h1>Videos</h1>

		<a name="medias-videos-general" />
		<h2>General videos on BCI and OpenViBE</h2>

		<script language=javascript type='text/javascript'>
			video("video.php?q=video-openvibe-promo-long-en", "openvibe-video-introduction.png", "This 4mn video is a quick overall presentation of what our software is an how it can be used to quickly build Brain Computer Intefaces.");
			video("video.php?q=video-openvibe-introduction-en", "openvibe-video-demonstrators.png", "This 10mn video is a global overview of what a Brain Computer Interface is and how OpenViBE was used to interact with virtual environments.");
		</script>

		<a name="medias-videos-tutorials" />
		<h2>Tutorials</h2>

		<script language=javascript type='text/javascript'>
			video("video.php?q=video-designer-training-en&playlist=true", "openvibe-video-training-course-en.png", "In this 1h video, Vincent goes over the designer functionalities step by step. You will discover almost all current exciting features of the platform and of the designer (<b>english</b> version).");
			video("video.php?q=video-designer-training-fr&playlist=true", "openvibe-video-training-course-fr.png", "In this 1h video, Yann goes over the designer functionalities step by step. You will discover almost all current exciting features of the platform and of the designer (<b>french</b> version).");
		</script>

		<a name="medias-screenshots" />
		<h1>Screenshots</h1>

		<a name="medias-screenshots-vr-applications" />
		<h2>Virtual Reality Applications</h2>

		<script language=javascript type='text/javascript'>
			screenshot("openvibe-snapshot-vr-app-01.png", "This is a simple VR application called \"<b>Use the force</b>\". In this first application, the user is asked to imagine feet movements. The beta synchronisation is used to lift the <i>Tie Fighter</i>.");
			screenshot("openvibe-snapshot-vr-app-02.png", "This is a more complex VR application called \"<b>Hand Ball</b>\". In this second application, the user is asked to imagine left or right hand movements. Brain activity triggered by motor imagery is then classified to move the ball towards the left or right goal.");
		</script>

		<a name="medias-screenshots-applications" />
		<h2>Other Applications</h2>

		<script language=javascript type='text/javascript'>
			screenshot("openvibe-snapshot-app-01.png", "This application is a kind of virtual keyboard or speller. It is based on a flashing matrix of letters and numbers. As the user counts the number of times the letter he intends to select is flashed, a specific brain wave, the P300, is emitted and can be detected. The application attempts at detecting the line and the column which contain the current target letter, and then deduces the letter the user wants to type.");
			screenshot("openvibe-snapshot-app-02.png", "This application is a neurofeedback application. It computes two levels of brain activity and compares them. The objective is for the user to train his brain on a specific brain activity over another specific brain activity.");
			screenshot("openvibe-snapshot-app-03.png", "This scenario shows how the voxel display plugin can be used to visualize brain activity sources, as reconstructed using inverse solution such as sLoreta applied on a discretized brain volume. This example is based on a grid of 2394 voxels generated from a probabilistic atlas, and represents a coarse discretization of the grey matter of the brain. Here, a spatial filter computes voxel activitation levels from potentials received from 60 EEG channels.");
		</script>

		<a name="medias-screenshots-designer" />
		<h2>OpenViBE Designer</h2>

		<script language=javascript type='text/javascript'>
			screenshot("openvibe-snapshot-designer-01.png", "This is the main designer dialog. On the left is the graphical area where scenarios will be created. On the right are the available processing boxes. The author simply has to drag & drop the boxes onto the graphical area. Double clicking a box will display its configuration parameters.");
			screenshot("openvibe-snapshot-designer-02.png", "This screenshot demonstrates how topographical information can be displayed, both in 2D and 3D with a simple processing pipeline.");
			screenshot("openvibe-snapshot-designer-03.png", "In this very ligt scenario, a spectral analysis is performed on a moving window. Spectrum data is displayed thanks to the power spectrum display and the time frequency map display boxes.");
			screenshot("openvibe-snapshot-designer-04.png", "This screenshot demonstrates how raw signals can be displayed thanks to the signal display. The signal may be zoomed in and out by the operator.");
			screenshot("openvibe-snapshot-designer-05.png", "This screenshot shows a more complex scenario performing 2-class motor imagery analysis for left and right hand movements. It can be used online with a trained classifier, e.g. to interact with a VR application.");
			screenshot("openvibe-snapshot-designer-06.png", "On this picture, one can see the Graz display in action, showing a cross as well as user feedback computed from his brain activity. In this case, the user seems to be imagining a left hand movement !");
		</script>

		<a name="medias-screenshots-acquisition-server" />
		<h2>OpenViBE Acquisition Server</h2>

		<script language=javascript type='text/javascript'>
			screenshot("openvibe-snapshot-acquisition-server-01.png", "The main acquisition server dialog. One can choose between the different available drivers and initialize the server for connections.");
			screenshot("openvibe-snapshot-acquisition-server-02.png", "Those two dialogs show how an acquisition driver can be configured and how electrode names can be filled in order to label signals.");
		</script>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="publications">

		<h1>Publications</h1>

		<p>The main publications based on or using OpenViBE software include :</p>
		<ul>
			<li>Y. Renard, F. Lotte, G. Gibert, M. Congedo, E. Maby, V. Delannoy, O. Bertrand, A. Lécuyer, "OpenViBE: An Open-Source Software Platform to Design, Test and Use Brain-Computer Interfaces in Real and Virtual Environments", Presence : teleoperators and virtual environments, vol. 19, no 1, 2010 (in press) (<a href="http://www.hal.inserm.fr/docs/00/47/71/53/PDF/Renard_et_al_2010_draft.pdf"> pdf version</a>)</li>
			<li>F. Lotte, A. Van Langhenhove, F. Lamarche, T. Ernest, Y. Renard, B. Arnaldi, A. Lécuyer, "Exploring Large Virtual Environments by Thoughts using a Brain-Computer Interface based on Motor Imagery and High-Level Commands", Presence: teleoperators and virtual environments, vol. 19, no. 1, 2010 (in press)(<a href="http://hal.archives-ouvertes.fr/docs/00/44/56/14/PDF/presence_2010_Museum_revisedDraft.pdf"> pdf version</a>)</li>
			<li>J.B. Sauvan, A. L&eacute;cuyer, F. Lotte, G. Casiez, "A Performance Model of Selection Techniques for P300-Based Brain-Computer Interfaces", (note) ACM SIGCHI Conference on Human Factors in Computing Systems (ACM CHI), 2009 (<a href="http://sites.google.com/site/fabienlotte/publications/CHI09.pdf?attredirects=0">pdf version</a>)</li>
			<li>F. Lotte, "Study of Electroencephalographic Signal Processing and Classification Techniques towards the use of Brain-Computer Interfaces in Virtual Reality Applications", PhD Thesis from the National Institute of Applied Sciences (INSA) Rennes, 2008 (<a href="http://tel.archives-ouvertes.fr/docs/00/35/71/65/PDF/PhDThesis_Lotte_2008.pdf">pdf version</a>)</li>
			<li>A. L&eacute;cuyer, F. Lotte, R. Reilly, R. Leeb, M. Hirose, M. Slater, "Brain-Computer Interfaces, Virtual Reality, and Videogames", IEEE Computer, vol. 41, no. 10, pp 66-72, 2008 (<a href="http://www.irisa.fr/bunraku/GENS/alecuyer/Lecuyer_computer_draft.pdf">pdf version</a>)</li>
			<li>F. Lotte, Y. Renard, A. L&eacute;cuyer, "Self-paced Brain-Computer Interaction with Virtual Worlds: a Quantitative and Qualitative Study "Out of the Lab", 4th International Brain-Computer Interface Workshop and Training Course, 2008, pp. 373-378 (<a href="http://hal.archives-ouvertes.fr/docs/00/30/43/40/PDF/bci-wtc08.pdf">pdf version</a>)</li>
		</ul>
		<p>More publications from partners of the OpenViBE project can be found the <a href="http://www.irisa.fr/bunraku/OpenViBE/wiki/index.php?title=Publications" class="leaving-link" onclick="window.open(this.href,'openvibe-wiki'); return false;">research project website</a>.</p>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="contributions">

		<h1>Want to Contribute ?</h1>

		<p>There are several ways you can contribute to the platform.</p>
		<p>If you plan to contribute, please contact us early on, so that we can first discuss the most appropriate way
		to do it and how your contribution fits in our development roadmap. This way, the community will probably benefit more from your work.</p>

		<h2>Testing the software</h2>

		<p>You can help us testing the software and provide us with your own report. We would be interested in the following :</p>
		<ul>
			<li>Testing OpenViBE on new architectures</li>
			<li>Testing OpenViBE on new operating systems</li>
			<li>Testing new functionalities</li>
		</ul>

		<h2>Reporting bugs</h2>

		<p>We would like to stress the importance of filling a <a href="http://openvibe.inria.fr/tracker/bug_report_page.php" class="leaving-link" onclick="window.open(this.href,'openvibe-tracker'); return false;">bug report</a> if unfortunately, you do find a bug.</p>
		<p>In any case, be aware that you can also notify us that everything works just fine and suits your needs :)</p>

		<h2>Developing additional features and correcting bugs</h2>

		<p>You can share pieces of code that you think may be of interest for the community. There are several topics you can contribute on :</p>
		<ul>
			<li>Creating new boxes for signal processing</li>
			<li>Creating new boxes for data classification</li>
			<li>Creating new boxes for data visualisation</li>
			<li>Creating new boxes for file reading and writing</li>
			<li>Creating new ready-to-use scenarios</li>
			<li>Creating new acquisition driver(s) for your device(s)</li>
			<li>Write more documentation and tutorials</li>
			<li>Package the software for your architecture (for example <em>RPM</em>s or <em>DEB</em>s on Linux distributions)
		</ul>

		</p>There are three levels this can be done :</p>
		<ul>
			<li>Every user can have read-only access to the forge. Such users may develop custom functionalities locally and submit a patch to the
			<a href="https://gforge.inria.fr/mail/?group_id=276" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">mailing lists</a> or to the
			<a href="http://openvibe.inria.fr/forum" class="leaving-link" onclick="window.open(this.href,'openvibe-forge'); return false;">forum</a> to be included in the repository. For
			Windows users having <i>TortoiseSVN</i>, just right click on the root folder, choose "create a patch..." and send us the file. For
			Linux users, such a patch can be created by using the following command in the root folder : <p><div class="code">svn diff &gt; patch.txt</div></p></li>
			<li>In case you submit several working patches and enhancements, you will be proposed to have restricted write access to the forge. People working in
			this configuration can work in their own branches and commit their changes directly to the forge. It is up to core developers to decide whether
			or not a branch is ready for integration to the trunk version.</li>
			<li>The last level is for core developers. They have full access to the forge. This means they can modify any file, including those in the trunk (current unstable version of the software) and tags.</li>
		</ul>

		<h2>Building installers and precompiled packages</h2>

		<p>As you probably noticed in the <a href="?q=download">download</a> section, several installers and precompiled packages are
		missing. If you have the knowledge to build such packages, feel free to contact us.</p>

		<h1>Contributors</h1>

		<p>OpenViBE was initially developed as a joint effort by <a href="http://www.inria.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">INRIA</a> and <a href="http://www.inserm.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">INSERM</a>
		laboratories and supported by the French <a href="http://www.agence-nationale-recherche.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">National Research Agency</a> within a consortium composed of 6 partners:
		<a href="http://www.inria.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">INRIA</a>,
		<a href="http://www.inserm.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">INSERM</a>,
		<a href="http://www.gipsa-lab.inpg.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">GIPSA-Lab</a>,
		<a href="http://www-list.cea.fr/" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">CEA-List</a>,
		<a href="http://www.afm-france.org/" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">AFM</a> and
		<a href="http://www.orange.com" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">Orange Labs</a>.</p>
		<p>It is currently maintained by the <a href="http://www.irisa.fr/vr4i" class="leaving-link" onclick="window.open(this.href,'openvibe-collaborator'); return false;">VR4I</a> team at INRIA.</p>

		<h2>Project Leader</h2>

		<ul><li>Anatole L&eacute;cuyer, INRIA (<i>anatole dot lecuyer at inria dot fr</i>)</li></ul>

		<h2>Lead Software Engineer</h2>

		<ul><li>Laurent Bonnet, INRIA (<i>laurent dot bonnet at inria dot fr</i>)</li></ul>

		<h2>INRIA Support and Development Team</h2>

		<ul>
			<li>Laurent Bonnet, lead software engineer, INRIA VR4I Team (<i>laurent dot bonnet at inria dot fr</i>)</li>
			<li>Jozef Leg&eacute;ny, software engineer, INRIA VR4I Team (<i>jozef dot legeny at inria dot fr</i>)</li>
			<li>Baptiste Payan, software engineer, INRIA Cortex Team (<i>baptiste dot payan at loria dot fr</i>)</li>
			<li>Guillermo Andrade Barroso, research engineer, INRIA SED Team (<i>guillermo dot andrade at inria dot fr</i>)</li>
			<li>Olivier Rochel, research engineer, INRIA SED Team (<i>olivier dot rochel at loria dot fr</i>)</li>
			
		</ul>

		<h2>Developers</h2>

		<ul>
			<li>Laurent Bonnet, software engineer, INRIA (<i>laurent dot bonnet at inria dot fr</i>)</li>
			<li>Jozef Leg&eacute;ny, software engineer, INRIA (<i>jozef dot legeny at inria dot fr</i>)</li>
			<li>Baptiste Payan, software engineer, INRIA (<i>baptiste dot payan at loria dot fr</i>)</li>
			<li>Gelu Ionescu, research engineer, GIPSA-Lab (<i>gelu dot ionescu at lis dot inpg dot fr</i>)</li>
			<li>Matthieu Goyat, software engineer, GIPSA-Lab (<i>matthieu dot goyat at gipsa-lab dot grenoble-inp dot fr</i>)</li>
			
			Previous developers:
			<li>Yann Renard, lead software engineer, INRIA</li>
			<li>Aur&eacute;lien Van Langhenhove, research engineer, INRIA </li>
			<li>Fabien Lotte, doctoral student, INRIA </li>
			<li>Vincent Delannoy, software engineer, INRIA </li>
			<li>Guillaume Gibert, postdoctoral fellow, INSERM </li>
			<li>Bruno Renier, trainee, INRIA</li>
			<li>Jean Baptiste Sauvan, trainee, INRIA</li>
			<li>Pierre Emmanuel Aguera, research engineer, INSERM </li>
			<li>Emmanuel Maby, postdoctoral fellow, INSERM </li>
			<li>Guillaume Lio, software engineer, GIPSA-Lab </li>
		</ul>

		<h2>Scientific Contributors</h2>

		<ul>
			<li>Anatole L&eacute;cuyer, INRIA (<i>anatole dot lecuyer at irisa dot fr</i>)</li>
			<li>Olivier Bertrand, senior researcher, INSERM</li>
			<li>Marco Congedo, research scientist, GIPSA-Lab</li>
			<li>Antoine Souloumiac, research engineer, CEA-List</li>
			<li>J&eacute;r&eacute;mie Mattout, research scientist, INSERM</li>
			<li>Christian Jutten, senior researcher, GIPSA-Lab</li>
			<li>Laurent Bougrain, assistant professor, INRIA Cortex Team</li>

			Previous scientific contributors:
			<li>Denis Ch&ecirc;ne, senior expert, Orange Labs</li>
			<li>Virginie Attina, INSERM</li>
			<li>Cedric Gouy-Pailler, doctoral student, GIPSA-Lab</li>
			<li>Ornella Plos, doctoral student, AFM</li>
			<li>Bertrand Rivet, postdoctoral fellow, CEA-List</li>
			<li>Bernard Hennion, senior researcher, Orange Labs</li>
		</ul>
		
		<h2>Community Contributors</h2>

		<ul>
			<li>Dieter Devlaminck</li>
			<li>Joan Fruitet</li>
			<li>Laurent George, PhD student, INRIA VR4I Team</li>
		</ul>

		<h1>OpenViBE Users</h1>

		<p>OpenViBE has already been tested by :</p>

		<ul>
			<li><a href="http://www.inria.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">INRIA Renne - Bretagne Atlantique</a>, France (Bunraku team)</li>
			<li><a href="http://www.inserm.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">INSERM</a>, France</li>
			<li><a href="http://www.gipsa-lab.inpg.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">GIPSA-Lab</a>, France</li>
			<li><a href="http://www-list.cea.fr/" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">CEA-List</a>, France</li>
			<li><a href="http://www.inria.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">INRIA Nancy - Grand Est Research Centre</a>, France (Cortex team)</li>
			<li><a href="http://www.inria.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">INRIA Sophia Antipolis - Mediterranee</a>, France (Odysee team)</li>
			<li><a href="http://lagis.ec-lille.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">LAGIS</a>, France</li>
			<li><a href="http://www.college-de-france.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">College de France</a>, France</li>
			<li><a href="http://www.insa-rouen.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">INSA Rouen</a>, France</li>

		<!-- FIXME -->

			<li><a href="http://www.fraunhofer.de/EN" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">Fraunhofer</a>, Germany</li>
			<li><a href="http://www.i2r.a-star.edu.sg" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">I2R A-Star</a>, Singapore</li>
			<li><a href="http://cogimage.dsi.cnrs.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">LENA</a>, France</li>
			<li><a href="http://www.swinburne.edu.au" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">Swinburne University of Technology</a>, Australia</li>
			<li><a href="https://www.sharjah.ac.ae" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">University of Sharjah</a>, United Arab Emirates</li>
			<li><a href="http://ewi.utwente.nl" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">University of Twente</a>, Netherlands</li>
			<li><a href="http://www.u-bordeaux2.fr" class="leaving-link" onclick="window.open(this.href,'openvibe-users'); return false;">Universit&eacute; de Bordeaux 2</a>, France</li>
			<li>...</li>
		</ul>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="related">

		<h1>Related Projects</h1>

		<p>The following projects are related to the OpenViBE software.</p>
		<ul>
			<li><a class="leaving-link" onclick="window.open(this.href,'openvibe-related'); return false;" href="related/adt-loic">ADT LOIC</a> (this website is in french)</li>
			<!-- <li><a class="leaving-link" onclick="window.open(this.href,'openvibe-related'); return false;" href="related/adt-immersive-bci">ADT Immersive BCI</a></li> -->
			<li><a class="leaving-link" onclick="window.open(this.href,'openvibe-related'); return false;" href="related/anr-openvibe1">ANR OpenViBE 1</a> (this project is over)</li>
			<!-- <li><a class="leaving-link" onclick="window.open(this.href,'openvibe-related'); return false;" href="related/anr-openvibe2">ANR OpenViBE 2</a></li> -->
			<!-- <li><a class="leaving-link" onclick="window.open(this.href,'openvibe-related'); return false;" href="related/anr-robik">ANR Robik</a></li> -->
		</ul>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="datasets">

		<h1>Introduction</h1>

		<p>During the past experiments, we recorded several datasets under different circumstances. These datasets
		can be downloaded for free in the hope that they can help research on signal processing. They have all been
		recorded using OpenViBE.</p>

		<h1>Register for Datasets Download</h1>

		<p>Before you can access to the download page, please register a few information so we can have a feedback
		about who you are and what you want to do. This could help us in proposing other datasets...</p>
		<p>The information you enter in this form won't be used to send you emails or so. Please consider filling
		it correctly.</p>

		<form method="post" action="index.php?q=datasets-download" class="register" name="register" onsubmit="return checkRegisterForm();">
			<table width=100% border=0>
				<tr><td></td><td width=70%></td><td></td></tr>
				<tr><td>name:</td><td><input type="text" name="name" class="register" style="width:100%" /></td><td><span style="text-color:red">*</span></td></tr>
				<tr><td>e-mail:</td><td><input type="text" name="email" class="register" style="width:100%" /></td><td><span style="text-color:red">*</span></td></tr>
				<tr><td>country:</td><td><input type="text" name="country" class="register" style="width:100%" /></td><td><span style="text-color:red">*</span></td></tr>
				<tr><td>are you a:</td><td><select name="entity" class="register" style="width:100%" /><option value="lab" selected="true">lab</option><option value="company">company</option><option value="school">school</option><option value="individual">individual</option><option name="other">other</option></select></td><td><span style="text-color:red">*</span></td></tr>
				<tr><td>lab / company / school name:</td><td><input type="text" name="entityname" class="register" style="width:100%" /></td><td><span style="text-color:red">*</span></td></tr>
				<tr><td>other information you'd like to give to us:</td><td><textarea name="otherinfo" class="register" style="width:100%" rows="8"></textarea></td></td><td></tr>
				<tr><td></td><td><center><input type="submit" class="register" value="proceed to download" /></center></td><td></td></tr>
			</table>
		</form>

	</div>

	<div class="content" id="datasets-download">

		<h1>Introduction</h1>

		<p>During the past experiments, we recorded several datasets under different circumstances. These datasets
		can be downloaded for free in the hope that they can help research on signal processing. They have all been
		recorded using OpenViBE.</p>

		<h1>Download Datasets</h1>

		<h2>Dataset #1 - Motor Imagery of Hands<br><span style="font-size:x-small">Recorded 2008/03</span><br><span style="font-size:x-small">Uploaded 2010/03</span></h2>

		<p>This datasets includes 14 records of left and right hand motor iomagery.</p>
		<p>They include 11 channels : C3, C4, Nz, FC3, FC4, C5, C1, C2, C6, CP3 and CP4.</p>
		<p>The channels are recorded in common average mode and Nz can be used as a reference if needed.</p>
		<p>The signal is sampled at 512 Hz and was recorded with our Mindmedia NeXus32B amplifier.</p>
		<p>Each file consists in 40 trials where the subject was requested to imagine either left or right hand movements (20 each).</p>
		<p>The experiment followed the Graz University protocol as described in [ref].</p>
		<p>The files were recorded on three different days of the same month.</p>
		<p>There are 3 CSV files you can download for each record :</p>
		<ul>
			<li>signal: contains the raw signal of the experiment.</li>
			<li>labels: contains the stimulation codes that happened during the experiment.</li>
			<li>spectrum: contains the spectrum of the signal computed on 1 second moving windows.</li>
		</ul>
		<p>Please note that stimulation code signification can be retrieved from <a href=http://openvibe.inria.fr/documentation/unstable/Doc_Stimulations.html>dedicated documentation page</a>.</p>

		<div class="dataset">
		<?php

			$dirname='i68Xg0jL6Z4JJ7cBQSl4y';

			$dir=opendir($dirname);
			$files=array();
			while(($file=readdir($dir))!=false)
			{
				array_push($files, $file);
			}
			closedir($dir);
			sort($files);

			$i=0;
			echo "<table class='dataset3'><tr>";
			foreach($files as $file)
			{
				if($file!="." && $file!="..")
				{
					$_filename=$dirname."/".$file;
					$_filesize=intval((filesize($_filename)*10)/(1024*1024))/10;
					echo "<td><a href=".$_filename.">".$file."</a> ";
					if($_filesize>0)
					{
						echo "(".$_filesize."mb)";
					}
					else
					{
						echo "(<1mb)";
					}
					echo "</td>";
					$i++;
					if($i==3)
					{
						echo "</tr><tr>";
						$i=0;
					}
				}
			}
			echo "</tr></td></table>";

		?>
		</div>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

	<div class="content" id="contact">

		<h1>Want to Contact us ?</h1>

		<h2>Project Leader</h2>

		<ul><li>Anatole L&eacute;cuyer, INRIA (<i>anatole dot lecuyer at irisa dot fr</i>)</li></ul>

		<h2>Lead Software Engineer</h2>

		<ul><li>Yann Renard, INRIA (<i>yann dot renard at irisa dot fr</i>)</li></ul>

	</div>

	</div>

<!------------------------------------------------------------------------------------------------------------------------------------------------------------------------>

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
</html>
