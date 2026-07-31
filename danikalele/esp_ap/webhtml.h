const char PAGE_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        .button {
            background-color: grey;
            border: 2px solid grey;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
        }
        .button2 {
            background-color: green;
        }
       .button.active {
            background-color: green;
        }
        nav {
            background-color: #333;
        }
        ul {
            list-style-type: none;
            margin: 0;
            padding: 0;
            overflow: hidden;
        }
        li {
            float: left;
        }
        a {
            display: block;
            color: black;
            text-align: center;
            padding: 14px 16px;
            text-decoration: none;
        }
        .bordered-text {
            border: 1px solid black;
            padding: 10px;
            margin: 10px;
            width: 60%;
            max-width: 150px;
            display: inline-block;
        }
    </style>
</head>
<body>
    <center>
      <h1>KNittelfeld Eisenbahnmuseum</h1>
      <h2>Drucke auf die Knopfe um die Sounds zu steueren</h2>
      <nav>
        <ul>
          <li><a href="#homePage"     onclick="toggleTabs('homePage')">Home</a></li>
          <li><a href="#servicesPage" onclick="toggleTabs('servicesPage')">Services</a></li>
          <li><a href="#counterPage"  onclick="toggleTabs('counterPage')">Counter</a></li>
        </ul>
      </nav>
        <div id="homePage">
            <div id="homeContent">
                <button class="button" name="1"                 value="OFF" type="submit" onclick="b1Press(this)">1</button>
                <button class="button" name="2"                 value="OFF" type="submit" onclick="b2Press(this)">2</button>
                <button class="button" name="3"                 value="OFF" type="submit" onclick="b3Press(this)">3</button>
                <button class="button" name="Zufallswiedergabe" value="OFF" type="submit" onclick="zufPress(this)">Zufall</button><br><br>
                <button class="button" name="4"                 value="OFF" type="submit" onclick="b4Press(this)">4</button>
                <button class="button" name="5"                 value="OFF" type="submit" onclick="b5Press(this)">5</button>
                <button class="button" name="6"                 value="OFF" type="submit" onclick="b6Press(this)">6</button>
                <button class="button" name="Test"              value="OFF" type="submit" onclick="testPress(this)">Test</button>
            </div>
        </div>

        <div id="servicesPage" style="display: none;">
            <h1>Lautstärke</h1>
            <button class="button" onclick="voldec()">-</button>
            <div class="bordered-text" id="volumeText">
                <h3>50%</h3>
            </div>
            <button class="button" onclick="volinc()">+</button>
            <h1>Deadtime</h1>
            <button class="button" onclick="decreaseTime()">-</button>
            <input type="text" id="timeField" value="0:30" readonly>
            <button class="button" onclick="increaseTime()">+</button>
        </div>
  
        <div id="counterPage" style="display: none;">
            <h3>Lichtschranke:</h3>
            <div class="bordered-text" id = "photob">
                
            </div>
            <h3>Bewegungsmelder:</h3>
            <div class="bordered-text" id = "motdetb">
               
            </div>
        </div>
    </center>

    <script>
        // global variable visible to all java functions
        var xmlHttp=createXmlHttpObject();

        // function to create XML object
        function createXmlHttpObject(){
          if(window.XMLHttpRequest){
            xmlHttp=new XMLHttpRequest();
          }
          else{
            xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
          }
          return xmlHttp;
        }
      
        function b1Press(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "b1Press", false);
            xhttp.send();
        }
        function b2Press(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "b2Press", false);
            xhttp.send();
        }
        function b3Press(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "b3Press", false);
            xhttp.send();
        }
        function b4Press(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "b4Press", false);
            xhttp.send();
        }
        function b5Press(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "b5Press", false);
            xhttp.send();
        }
        function b6Press(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "b6Press", false);
            xhttp.send();
        }
        function zufPress(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "zufPress", false);
            xhttp.send();
        }
        function testPress(btn) {
            let buttons = document.getElementsByClassName("button");
            for (let i = 0; i < buttons.length; i++) {
                buttons[i].classList.remove("active");
            }
            btn.classList.add("active");
            var xhttp = new XMLHttpRequest(); 
            xhttp.open("PUT", "testPress", false);
            xhttp.send();
        }
       
      
        let currentVolume = 50;
        function volinc() {
            currentVolume++;
            if (currentVolume < 0) currentVolume = 0;
            if (currentVolume > 100) currentVolume = 100;
            document.getElementById("volumeText").innerHTML = `<h3>${currentVolume}%</h3>`;
            xhttp.open("PUT", "volinc", false);
            xhttp.send();
        }
        function voldec() {
            currentVolume--;
            if (currentVolume < 0) currentVolume = 0;
            if (currentVolume > 100) currentVolume = 100;
            document.getElementById("volumeText").innerHTML = `<h3>${currentVolume}%</h3>`;
            xhttp.open("PUT", "voldec", false);
            xhttp.send();
        }

        function increaseTime() {
            let timeField = document.getElementById("timeField");
            let timeValue = timeField.value.split(":");
            let minutes = parseInt(timeValue[0]);
            let seconds = parseInt(timeValue[1]);

            let newMinutes = minutes;
            let newSeconds = seconds + 30;

            if (newSeconds >= 60) {
                newMinutes += 1;
                newSeconds -= 60;
            }

            timeField.value = newMinutes.toString().padStart(2, '0') + ':' + newSeconds.toString().padStart(2, '0');
            
            xhttp.open("PUT", "inctime", false);
            xhttp.send();
        }

        function decreaseTime() {
            let timeField = document.getElementById("timeField");
            let timeValue = timeField.value.split(":");
            let minutes = parseInt(timeValue[0]);
            let seconds = parseInt(timeValue[1]);

            let newMinutes = minutes;
            let newSeconds = seconds - 30;

            if (newSeconds < 0) {
                newMinutes -= 1;
                newSeconds += 60;
            }

            if (newMinutes < 0) {
                newMinutes = 0;
                newSeconds = 0;
            }

            timeField.value = newMinutes.toString().padStart(2, '0') + ':' + newSeconds.toString().padStart(2, '0');

            xhttp.open("PUT", "dectime", false);
            xhttp.send();
        }

        function toggleTabs(tabId) {
            if (tabId === 'homePage') {
                document.getElementById('homePage').style.display = 'block';
                document.getElementById('servicesPage').style.display = 'none';
                document.getElementById('counterPage').style.display = 'none';
            } else if (tabId === 'servicesPage') {
                document.getElementById('homePage').style.display = 'none';
                document.getElementById('servicesPage').style.display = 'block';
                document.getElementById('counterPage').style.display = 'none';
            }  else if (tabId === 'counterPage') {
                document.getElementById('homePage').style.display = 'none';
                document.getElementById('servicesPage').style.display = 'none';
                document.getElementById('counterPage').style.display = 'block';
            }
        }

        function response(){
            // get the xml stream
            xmlResponse=xmlHttp.responseXML;
  
        } 
    </script>
</body>
</html>

)=====";
