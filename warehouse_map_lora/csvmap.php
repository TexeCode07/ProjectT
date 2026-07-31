
// csvmap.php
<?php
// Avoid caching for the file
header('Cache-Control: no-store, no-cache, max-age=0, must-revalidate, proxy-revalidate');
header('Cache-Control: post-check=0, pre-check=0', false);
header('Pragma: no-cache');

// Read the content of the 'door.csv' file into an array ($file), each line as an element
$file = file('door.csv');

// Extract the door numbers from the URL parameter and convert it to an array
$doorNumbers = explode(",", $_GET['doorNumbers']);

// Loop through each line in the CSV file
foreach ($file as $index => $line) {
   $doorDetails = explode(',', $line); // Split the line into an array by comma
   $doorNumber = intval(trim($doorDetails[0], 'DOOR')); // Extract the door number and convert it to an integer

   // Check if the door number is in the list to be updated
   if (in_array($doorNumber, $doorNumbers)) {
       $doorDetails[3] = '1'; // Modify the status to 1
   } else {
       $doorDetails[3] = '0'; // Set status to 0 for doors not in the list
   }
   $file[$index] = implode(',', $doorDetails). "\n"; // Update the line in the file
}

// Save the modified data back to the file
file_put_contents('door.csv', implode('', $file));

echo 'CSV file updated successfully!';
?>











