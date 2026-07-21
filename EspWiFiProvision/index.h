const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP Wi-Fi Manager</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="data:,">
  <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
    }

    body {
      margin: 0;
      background-color: #f4f4f4;
    }

    h1 {
      font-size: 1.8rem;
      color: white;
      margin: 0;
      padding: 1rem;
    }

    .topnav {
      background-color: #0A1128;
      overflow: hidden;
    }

    .content {
      padding: 2rem;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: calc(100vh - 50px); /* Adjust height to center form */
    }

    .card {
      background-color: white;
      box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, 0.5);
      border-radius: 8px;
      padding: 2rem;
      width: 100%;
      max-width: 400px;
    }

    label {
      font-size: 1.2rem;
      display: block;
      margin-bottom: 0.5rem;
      color: #034078;
      text-align: left;
    }

    input[type="text"] {
      width: 100%;
      padding: 10px;
      margin-bottom: 1rem;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
      font-size: 1rem;
    }

    input[type="submit"] {
      width: 100%;
      padding: 10px;
      border: none;
      color: white;
      background-color: #034078;
      font-size: 1.2rem;
      border-radius: 4px;
      cursor: pointer;
      transition: 0.3s;
    }

    input[type="submit"]:hover {
      background-color: #1282A2;
    }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>ESP Wi-Fi Manager</h1>
  </div>
  <div class="content">
    <div class="card">
      <form action="/" method="POST">
        <label for="ssid">SSID</label>
        <input type="text" id="ssid" name="ssid" placeholder="Enter Wi-Fi SSID" required>
        
        <label for="pass">Password</label>
        <input type="text" id="pass" name="pass" placeholder="Enter Wi-Fi Password" required>
        <input type="submit" value="Submit">
      </form>
    </div>
  </div>
</body>
</html>
)rawliteral";