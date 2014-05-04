<?php
    require_once("../php/CodeSystemType.php");
    $systemType = new Code_SystemType();
?>
<!DOCTYPE html>
<html>
<head>
  <title>设备</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <!-- Bootstrap -->
  <link rel="stylesheet" href="../css/bootstrap.min.css" type="text/css">
  <link rel="stylesheet" href="../css/bootstrap-theme.min.css" type="text/css">
  <link rel="stylesheet" href="../css/Style.css" type="text/css"></head>
  <script src="http://cdn.bootcss.com/jquery/1.9.1/jquery.min.js"></script>
  <script type="text/javascript">
    function saveReport() {  
    // jquery 表单提交  
    $("#SaveFrom").Submit(function(message) {  
          // 对于表单提交成功后处理，message为提交页面saveReport.htm的返回内容  
          alert(message);
       });  
      
    return false; // 必须返回false，否则表单会自己再做一次提交操作，并且页面跳转  
    }     
  </script>
<body>
  <div class="container">
    <div class="row">
      <div class="col-md-12">
        <ul class="nav nav-tabs">
          <li >
            <a href="../">首页</a>
          </li>
          <li class="active">
            <a href="">数据字典</a>
          </li>
          <li >
            <a href="">设备管理</a>
          </li>
          <li >
            <a href="">关于</a>
          </li>
        </ul>
      </div>
      <div class="leaderboard">
        <h1>数据字典</h1>
        <p>数据字典balabalabala</p>

      </div>
      <div class="row">
        <div class="col-md-4"></div>
        <div class="col-md-4">
          <h2>设备信息</h2>
          <?php
            $systemType->
          Creat_Table();
        ?>
        </div>
        <div class="col-md-4">
          <h2>Ruby Apps</h2>
          <p>
            Donec id elit non mi porta gravida at eget metus. Fusce dapibus, tellus ac cursus commodo, tortor mauris condimentum nibh,ut fermentum massa justo sit amet risus. Etiam porta sem malesuada magna mollis euismod. Donec sed odio dui.
          </p>
          <p>
            <a class="btn btn-success btn-large" href="#">进入</a>
          </p>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-md-4">
        <table class="table table-bordered table-hover">
          <tr>
            <td>#</td>
            <td>系统名称</td>
            <td>说明</td>
          </tr>
          <tr  class="success">
            <td>1</td>
            <td>系统名称</td>
            <td>说明111</td>
          </tr>
        </table>
        <form id="showDataForm" action="InsertTollstation.php" method="post" >
          Name:
          <input type="text" name="name" />
          LC:
          <input type="text" name="LC" />
          FL:
          <input type="text" name="FL" />
          <input type="submit" value="保存"/>
        </form>
      </div>
      <div class="col-md-4">
        <?php   
    echo "<h2>设备信息</h2>
      ";    
    $systemType->Creat_Table();
  ?>
      <button class="btn btn-default" data-toggle="modal" data-target="#myModal" >Sign in</button>
      <!-- Modal -->
      <div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
        <div class="modal-dialog">
          <div class="modal-content">
            <div class="modal-header">
              <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
              <h4 class="modal-title" id="myModalLabel">系统设置</h4>
            </div>
            <div class="modal-body">
              <form id="SaveFrom" class="form-horizontal" action="../php/Code_SystemTypeInsert.php" method="post" role="form" onsubmit="return saveReport();">
                <div class="form-group">
                  <label for="inputEmail3" class="col-sm-3 control-label">系统名称</label>
                  <div class="col-sm-9">
                    <input type="text" class="form-control" id="inputEmail3" name="Name" ></div>
                </div>
                <div class="form-group">
                  <label for="inputPassword3" class="col-sm-3 control-label">说明</label>
                  <div class="col-sm-9">
                    <input type="text" class="form-control" id="inputPassword3" name="Desc" ></div>
                </div>

                <input type="submit"></form>

            </div>
          </div>
          <!-- /.modal-content --> </div>
        <!-- /.modal-dialog --> </div>
      <!-- /.modal --> </div>
    <div class="col-md-4"></div>
  </div>
  <footer>
    <p>&copy; Copyright 2013 by MoYuanMing Design.</p>
  </footer>
</div>
<script src="http://cdn.bootcss.com/jquery/1.9.1/jquery.min.js"></script>
<script src="http://cdn.bootcss.com/twitter-bootstrap/2.0.4/bootstrap.min.js"></script>

</body>

</html>