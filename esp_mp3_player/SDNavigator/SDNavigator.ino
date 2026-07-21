#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

// Define OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define Buttons
#define BUTTON_UP    12
#define BUTTON_DOWN  14
#define BUTTON_DONE  27

// SD Card
#define SD_CS 5 // Set this to your SD card's chip select pin

int menuIndex = 0;        // Currently selected item
int scrollOffset = 0;     // Starting index for the displayed items
const int maxDisplayItems = 4; // Number of items that can fit on the screen
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

String currentDirectory = "/";
String items[50]; // Files and folders
int itemCount = 0;

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize Buttons
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_DONE, INPUT_PULLUP);

  // Initialize SD Card
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card initialization failed!"));
    while (true);
  }
  Serial.println(F("SD Card initialized."));
  scanDirectory(currentDirectory);

  displayMenu();
}

void loop() {
  handleButtons();
  displayMenu();
}

// Scan current directory for files and folders
void scanDirectory(String directory) {
  Serial.print("Scanning directory: ");
  Serial.println(directory);

  File root = SD.open(directory.c_str());
  if (!root || !root.isDirectory()) {
    Serial.println(F("Failed to open directory or not a directory."));
    return;
  }

  itemCount = 0;
  items[itemCount++] = "Back"; // Add "Back" option

  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
    if (directory != "/") {
      fileName = directory + "/" + fileName;
    }
    items[itemCount++] = fileName;

    Serial.print("Found: ");
    Serial.println(fileName);

    file = root.openNextFile();
  }

  root.close();
  Serial.println(F("Directory scan complete."));
}

// Function to truncate long file names for display
String truncateName(String name, int maxLength) {
  if (name.length() > maxLength) {
    return name.substring(0, maxLength - 3) + "..."; // Truncate and add ellipsis
  }
  return name;
}


// Display menu with scrolling and truncated names
void displayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int endIndex = min(scrollOffset + maxDisplayItems, itemCount);
  for (int i = scrollOffset; i < endIndex; i++) {
    int displayIndex = i - scrollOffset; // Line position on screen
    if (i == menuIndex) {
      display.fillRoundRect(0, displayIndex * 16, 128, 14, 3, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(4, displayIndex * 16 + 2);
    if (i == 0) {
      display.print("Back");
    } else {
      String item = items[i];
      File file = SD.open(item.c_str());
      String displayName = item.substring(item.lastIndexOf('/') + 1); // Extract file/folder name
      displayName = truncateName(displayName, 20); // Adjust 20 based on screen width and font size
      // if (file.isDirectory()) {
      //   display.print("[DIR] ");
      // }
      display.print(displayName);
      file.close();
    }
  }

  display.display();
}

// Handle button presses with scrolling
void handleButtons() {
  if (millis() - lastButtonPress < debounceDelay) return;

  if (digitalRead(BUTTON_UP) == LOW) {
    lastButtonPress = millis();
    if (menuIndex > 0) {
      menuIndex--;
      if (menuIndex < scrollOffset) {
        scrollOffset--; // Scroll up
      }
    } else {
      menuIndex = itemCount - 1; // Wrap to last item
      scrollOffset = max(0, itemCount - maxDisplayItems);
    }
    Serial.println("Button UP pressed");
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    lastButtonPress = millis();
    if (menuIndex < itemCount - 1) {
      menuIndex++;
      if (menuIndex >= scrollOffset + maxDisplayItems) {
        scrollOffset++; // Scroll down
      }
    } else {
      menuIndex = 0; // Wrap to first item
      scrollOffset = 0;
    }
    Serial.println("Button DOWN pressed");
  }

  if (digitalRead(BUTTON_DONE) == LOW) {
    lastButtonPress = millis();
    Serial.print("Selected: ");
    Serial.println(items[menuIndex]);

    if (menuIndex == 0) {
      // "Back" selected
      if (currentDirectory != "/") {
        currentDirectory = currentDirectory.substring(0, currentDirectory.lastIndexOf('/'));
        if (currentDirectory.isEmpty()) {
          currentDirectory = "/";
        }
        scanDirectory(currentDirectory);
        menuIndex = 0;
        scrollOffset = 0;
      }
    } else {
      String selectedItem ="/"+ items[menuIndex];
      File file = SD.open(selectedItem.c_str());
      if (file.isDirectory()) {
        Serial.print("Entering directory: ");
        Serial.println(selectedItem);
        currentDirectory = selectedItem;
        scanDirectory(currentDirectory);
        menuIndex = 0;
        scrollOffset = 0;
      } else {
        // Handle file selection (e.g., play .mp3 file or open file)
        Serial.print("File selected: ");
        Serial.println(selectedItem);
      }
      file.close();
    }
  }
}
