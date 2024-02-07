Current status:<br>
Arduino will first connect the backend with the known WiFi SSID and WiFi password. After successfully connecting to the WiFi, Arduino will send the MAC address of the board to the backend. After receiving the status code 200, Arduino will send the push button message to the backend each time I push the button locally. If the status code is something else, Arduino will report an error connection to the backend and stop the connection. <br>

What we need to do: <br>
-let Arduino connect with the front end through Bluetooth. Front-end will send the WiFi SSID and WiFi password for the Arduino to connect to the WiFi. (First tiem connection) <br>
-Arduino will send the MAC address to the front end for the first-time connection.<br>
-Show the battery status <br>



