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

header("Location: http://www.google.com/search?q=".$g_v_argument_list['q']."+site%3Aopenvibe.inria.fr");

?>
