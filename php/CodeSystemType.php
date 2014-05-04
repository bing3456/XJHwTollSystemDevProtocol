<?php
	include "medoo.min.php";

	/**
	* 
	*/
	class Code_SystemType 
	{
		
		
		public function Creat_Table()
		{
			$database = new medoo("TollDSM");
			$datas = $database->select("Code_SystemType", "*");
			echo "<table class=\"table table-bordered table-hover\">";
			echo "<tr>
			<td>ID</td>
			<td>Name</td>
			<td>Desc</td>
			</tr> " ;
			foreach($datas as $data)
			{
			echo "<tr class=\"success\">" ;
			echo "<td>" . $data["ID"] . " </td>" ;
			echo "<td>" . $data["Name"] . " </td>" ;
			echo "<td>" . $data["Desc"] . " </td>" ;
			echo "</tr>" ;
			}
			echo "</table>";
		}
	}
	
?>