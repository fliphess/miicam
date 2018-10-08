<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width" />
    <title>Chuangmi 720P hack</title>
    <link rel="stylesheet" href="/css/chuangmi.css">
  </head>
  <body>
    <ul class="topnav">
      <li><a href="index.php">Home</a></li>
      <li><a href="settings.php">Settings</a></li>
      <li class="right"><a href="about.php" class="active">About</a></li>
    </ul>

    <div style="padding:0 16px;">
    <h1>Chuangmi 720P HTTP Server</h1>
    <p>This Chuangmi is powered by a custom firmware. See <a href="https://github.com/ghoost82/mijia-720p-hack"</a> for more informations</a>.</p>
    <h2>RTSP stream channels</h2>
    <p>This camera IP is <span id="ip"></span></p>
    <ul>
      <li><a id="ch0_0" href="">High definition video stream</a></li>
    </ul>
    <h2>Record files</h2>
    <ul>
      <li><a href="/media/">Click here to access the record files.</a></li>
    </ul>
    <h2>Log files</h2>
    <ul>
      <li><a href="/log/">Click here to access the log files.</a></li>
    </ul>
    <div>
    <script type="text/javascript">
      var ip = location.host;
      document.getElementById("ip").textContent=ip;
      document.getElementById("ch0_0").href="rtsp://"+ip+":554/live/ch00_0";
    </script>
  </body>
</html>
