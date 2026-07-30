#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <vector>
#include "driver/uart.h"
#include "esp_log.h"

#include <string>
#include <stdbool.h>

//reset varicon
unsigned char resetvaricon[]  =  {0x5A, 0xA5, 0x05, 0x82, 0x93, 0x80, 0x00, 0x08};
//change pages
unsigned char changePageTo0[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00};
unsigned char changePageTo1[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x01};
unsigned char changePageTo2[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x02};
unsigned char changePageTo3[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x03};
unsigned char changePageTo4[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x04};
unsigned char changePageTo5[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x05};
unsigned char changePageTo6[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x06};
unsigned char changePageTo7[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x07};
unsigned char changePageTo8[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x08};
unsigned char changePageTo9[]  =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x09};
unsigned char changePageTo10[] =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x0a};
unsigned char changePageTo13[] =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x0d};
unsigned char changePageTo14[] =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x0e};
unsigned char changePageTo15[] =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x0f};
unsigned char changePageTo16[] =  {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x10};


uint8_t scrollUpCmd[]   = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x10, 0x01, 0x00, 0x01};
uint8_t scrollDownCmd[] = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x10, 0x01, 0x00, 0x02};

uint8_t methEndpoint[]     = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x01};
uint8_t methKinetics[]     = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x02};
uint8_t methfixtime[]      = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x03};
uint8_t methAbsorbance[]   = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x04};
uint8_t patidApply[]       = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x05};
uint8_t patusernameApply[] = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x06};
uint8_t pattestnameApply[] = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x07};
uint8_t patdateApply[]     = {0x5A, 0xA5, 0x06, 0x83, 0x90, 0x00, 0x01, 0x00, 0x08};

//row click 
uint8_t row1click[] = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x80, 0x01, 0x00, 0x09};
uint8_t row2click[] = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x80, 0x01, 0x00, 0x0a};
uint8_t row3click[] = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x80, 0x01, 0x00, 0x0b};
uint8_t row4click[] = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x80, 0x01, 0x00, 0x0c};

//view
uint8_t viewpagedate[]     = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x85, 0x01, 0x00, 0x01};
uint8_t viewpagemethod[]   = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x85, 0x01, 0x00, 0x02};
uint8_t viewpagepatid[]    = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x85, 0x01, 0x00, 0x03};
uint8_t viewpageusername[] = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x85, 0x01, 0x00, 0x04};
uint8_t viewpagetestname[] = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x85, 0x01, 0x00, 0x05};
//delete
uint8_t deletepatientindexdate[]      = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x90, 0x01, 0x00, 0x01};
uint8_t deletepatientindexmethod[]    = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x90, 0x01, 0x00, 0x02};
uint8_t deletepatientindexpatid[]     = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x90, 0x01, 0x00, 0x03};
uint8_t deletepatientindexusername[]  = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x90, 0x01, 0x00, 0x04};
uint8_t deletepatientindextestname[]  = {0x5A, 0xA5, 0x06, 0x83, 0x93, 0x90, 0x01, 0x00, 0x05};

// VP base addresses for each row
uint16_t rowVPs[4][6] = {
    {0x9100, 0x9110, 0x9130, 0x9140, 0x9150, 0x9160},
    {0x9170, 0x9180, 0x91a0, 0x91b0, 0x91c0, 0x91d0},
    {0x91e0, 0x91f0, 0x9210, 0x9220, 0x9230, 0x9240},
    {0x9250, 0x9260, 0x9280, 0x9290, 0x92a0, 0x92b0}
};

// ---------------- Enums ----------------
enum SearchMode { SEARCH_NONE, SEARCH_METHOD, SEARCH_ID, SEARCH_USERNAME, SEARCH_TESTNAME, SEARCH_DATE};
SearchMode currentSearchMode = SEARCH_NONE;
typedef enum {
    ENDPOINT,
    KINETICS,
    FIXED_TIME,
    ABSORBANCE
} MethodType;

static const char *TAG = "PATIENT";

// ---------------- Patient Struct ----------------
struct PatientData {
    std::string patient_id;
    std::string test_name;
    std::string conc;
    std::string date;
    std::string time;
    MethodType method;
    std::string user_name;
};



const char* methodToString(MethodType m) {
    switch (m) {
        case ENDPOINT:   return "ENDPOINT";
        case KINETICS:   return "KINETICS";
        case FIXED_TIME: return "FIXED_TIME";
        case ABSORBANCE: return "ABSORBANCE";
        default:         return "UNKNOWN";
    }
}


std::vector<PatientData> patients;   


// UART settings
#define UART_PORT UART_NUM_1
#define TXD_PIN  (17)
#define RXD_PIN  (16)
#define BAUD_RATE 9600

void writeDataVariableDisplay(uint16_t vpAddr, const std::string &data, int fieldWidth = 16) {
    std::string out = data;

    // Pad with spaces to overwrite previous content
    while (out.length() < (size_t)fieldWidth) {
        out += " ";
    }

    int len = out.length();

    // LEN = 1 (cmd) + 2 (VP) + data length
    std::vector<uint8_t> packet(6 + len);

    packet[0] = 0x5A;
    packet[1] = 0xA5;
    packet[2] = len + 3;   // LEN field
    packet[3] = 0x82;      // Write variable
    packet[4] = vpAddr >> 8;
    packet[5] = vpAddr & 0xFF;

    // Copy ASCII characters of padded data into frame
    memcpy(&packet[6], out.c_str(), len);

    // Send using ESP-IDF UART driver
    uart_write_bytes(UART_PORT, (const char*)packet.data(), packet.size());
}

// ---------------- Globals ----------------
int currentPageStart = 0;
int totalResults = 0;
int matchIndices[20];
int selectedPatientIndex = -1;
int patientCount = 0;  // current number of patients

MethodType filterMethod;
std::string filterIdStr, filterTestNameStr, filterUserNameStr;
int filterDateStart;
int filterDateEnd;


// ---------------- Date conversion ----------------
int dateStringToInt_ddmmyy(const std::string &d) {
    if (d.size() != 8) return 0; // expect dd-mm-yy

    int dd = std::stoi(d.substr(0, 2));
    int mm = std::stoi(d.substr(3, 2));
    int yy = std::stoi(d.substr(6, 2));

    int year = yy + 2000;  // assume 20xx

    return year * 10000 + mm * 100 + dd; // e.g. "14-08-25" → 20250814
}

// ---------------- Build Matches ----------------
void buildMatches() {
    totalResults = 0;

    for (int i = 0; i < patients.size(); ++i) {
        if (currentSearchMode == SEARCH_METHOD) {
            if (patients[i].method == filterMethod) {
                matchIndices[totalResults++] = i;
            }
        } else if (currentSearchMode == SEARCH_ID) {
            if (patients[i].patient_id.find(filterIdStr) != std::string::npos) { // partial match
                matchIndices[totalResults++] = i;
            }
        } else if (currentSearchMode == SEARCH_USERNAME) {
            if (patients[i].user_name.find(filterUserNameStr) != std::string::npos) {
                matchIndices[totalResults++] = i;
            }
        } else if (currentSearchMode == SEARCH_TESTNAME) {
            if (patients[i].test_name.find(filterTestNameStr) != std::string::npos) {
                matchIndices[totalResults++] = i;
            }
        } else if (currentSearchMode == SEARCH_DATE) {
            int patientDate = dateStringToInt_ddmmyy(patients[i].date);
            if (patientDate >= filterDateStart && patientDate <= filterDateEnd) {
                matchIndices[totalResults++] = i;
            }
        }
    }
}

// ---------------- Print Current Page ----------------
void printCurrentPage() {
    if (totalResults == 0) {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 6; ++c)
                writeDataVariableDisplay(rowVPs[r][c], "          "); // send to DWIN

        ESP_LOGW(TAG, "No results found");
        return;
    }

    int lastPageStart = ((totalResults - 1) / 4) * 4;
    if (currentPageStart > lastPageStart) currentPageStart = lastPageStart;

    for (int row = 0; row < 4; ++row) {
        int idx = currentPageStart + row;
        if (idx < totalResults) {
            int p = matchIndices[idx];
            // Send patient info to DWIN over UART
            writeDataVariableDisplay(rowVPs[row][0], std::to_string(idx + 1));
            writeDataVariableDisplay(rowVPs[row][1], patients[p].patient_id, 16);
            writeDataVariableDisplay(rowVPs[row][2], patients[p].test_name, 10);
            writeDataVariableDisplay(rowVPs[row][3], patients[p].conc, 10);
            writeDataVariableDisplay(rowVPs[row][4], patients[p].date, 10);
            writeDataVariableDisplay(rowVPs[row][5], patients[p].time, 10);
        } else {
            for (int c = 0; c < 6; ++c)
                writeDataVariableDisplay(rowVPs[row][c], "          ");
        }
    }

    ESP_LOGI(TAG, "Page start=%d total=%d", currentPageStart, totalResults);
}


// ---------------- Search functions ----------------
void startSearchByDate(const std::string &startDate, const std::string &endDate) {
    currentSearchMode = SEARCH_DATE;
    filterDateStart = dateStringToInt_ddmmyy(startDate);
    filterDateEnd   = dateStringToInt_ddmmyy(endDate);
    currentPageStart = 0;
    buildMatches();
    printCurrentPage();
}

void startSearchByMethod(MethodType method) {
    currentSearchMode = SEARCH_METHOD;
    filterMethod = method;
    currentPageStart = 0;
    buildMatches();
    printCurrentPage();
}

void startSearchByIdPartial(const std::string &searchStr) {
    currentSearchMode = SEARCH_ID;
    filterIdStr = searchStr;
    currentPageStart = 0;
    buildMatches();
    printCurrentPage();
}

void startSearchByUserName(const std::string &searchStr) {
    currentSearchMode = SEARCH_USERNAME;
    filterUserNameStr = searchStr;
    currentPageStart = 0;
    buildMatches();
    printCurrentPage();
}

void startSearchByTestName(const std::string &searchStr) {
    currentSearchMode = SEARCH_TESTNAME;
    filterTestNameStr = searchStr;
    currentPageStart = 0;
    buildMatches();
    printCurrentPage();
}

// ---------------- Show row info ----------------

void showRowInfo(int row) {
    int idx = currentPageStart + row;
    if (idx < totalResults) {
        int p = matchIndices[idx];
        selectedPatientIndex = p;   // save for later use

        ESP_LOGI(TAG, "---- Row Selected ----");
        ESP_LOGI(TAG, "Patient ID: %s", patients[p].patient_id.c_str());
        ESP_LOGI(TAG, "Test Name : %s", patients[p].test_name.c_str());
        ESP_LOGI(TAG, "Conc      : %s", patients[p].conc.c_str());
        ESP_LOGI(TAG, "Date      : %s", patients[p].date.c_str());
        ESP_LOGI(TAG, "Time      : %s", patients[p].time.c_str());
        ESP_LOGI(TAG, "User      : %s", patients[p].user_name.c_str());
        ESP_LOGI(TAG, "----------------------");
    } else {
        ESP_LOGW(TAG, "Row has no data, ignoring click.");
        selectedPatientIndex = -1;
    }
}



// ===================== Scroll helpers =====================
void scrollDown() {
    if (totalResults == 0) return;

    if (currentPageStart + 4 < totalResults) {
        currentPageStart += 4;
    } else {
        currentPageStart = ((totalResults - 1) / 4) * 4;
    }

    ESP_LOGI(TAG, "Scrolled down → Page start=%d", currentPageStart);
    printCurrentPage();
}

void scrollUp() {
    if (totalResults == 0) return;

    if (currentPageStart >= 4) {
        currentPageStart -= 4;
    } else {
        currentPageStart = 0;
    }

    ESP_LOGI(TAG, "Scrolled up → Page start=%d", currentPageStart);
    printCurrentPage();
}


void deletePatientAtIndex(int idx, uint8_t* changePageCmd, size_t cmdLen) {
    if (idx >= 0 ) {
        int p = selectedPatientIndex;
        // Log before deleting
        ESP_LOGI(TAG, "Deleting patient with ID: %s", patients[p].patient_id.c_str());

        // Erase the patient at index p
        patients.erase(patients.begin() + p);

        // Reset selected index
        selectedPatientIndex = -1;

        ESP_LOGI(TAG, "Patient deleted successfully.");
        uart_write_bytes(UART_PORT, (const char*)changePageTo3, sizeof(changePageTo3));
    }
}

void viewPatientPage(int p, uint8_t *changePageCmd, size_t cmdLen) {
    if (p >= 0) {
        // Display patient data via your writeDataVariableDisplay function
        writeDataVariableDisplay(0x9400, patients[p].patient_id, 16);   // pat id
        writeDataVariableDisplay(0x9420, patients[p].test_name, 16);    // test name
        writeDataVariableDisplay(0x9430, patients[p].conc, 16);         // result

        // Concatenate date + time into a buffer
        char datetime[40];
        snprintf(datetime, sizeof(datetime), "%s %s", 
         patients[p].date.c_str(), 
         patients[p].time.c_str());

        writeDataVariableDisplay(0x9440, datetime, 16);

        // Convert method to string
        const char *methodStr = methodToString(patients[p].method);
        writeDataVariableDisplay(0x9450, methodStr, 16);

        writeDataVariableDisplay(0x9460, patients[p].user_name, 16);

       
        ESP_LOGI(TAG, "Sent selected patient to view page.\n");
        // Send UART page change
        uart_write_bytes(UART_PORT, (const char*)changePageCmd, cmdLen);
    } else {
        // ESP_LOGI(TAG, "No row selected, ignoring viewpage command.\n");
         ESP_LOGI(TAG, "No row selected (p=%d)(patcount=%d), ignoring viewpage command.", p,patientCount);
    }
}

// Command matching helper
bool matchCmd(uint8_t *buf, const uint8_t *cmd, int len) {
    for (int i = 0; i < len; i++) {
        if (buf[i] != cmd[i]) return false;
    }
    return true;
}

// Global variables
char idStr[16];
char testnameStr[10];
char usernameStr[10];
char dateStr1[20];
char dateStr2[20];

// Handle incoming command
void handleIncomingCmd(uint8_t *data, int len) {
    if (len == sizeof(scrollUpCmd) && matchCmd(data, scrollUpCmd, sizeof(scrollUpCmd))) {
        scrollUp();
    } else if (len == sizeof(scrollDownCmd) && matchCmd(data, scrollDownCmd, sizeof(scrollDownCmd))) {
        scrollDown();
    } 
    else if (len == sizeof(methEndpoint) && matchCmd(data, methEndpoint, sizeof(methEndpoint))){
        writeDataVariableDisplay(0x9030, "Endpoint");
        currentPageStart = 0;
        currentSearchMode = SEARCH_METHOD;
        startSearchByMethod(ENDPOINT);
        selectedPatientIndex = -1;
        uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
        uart_write_bytes(UART_PORT, (const char*)changePageTo4, sizeof(changePageTo4));
    } else if (len == sizeof(methKinetics) && matchCmd(data, methKinetics, sizeof(methKinetics))) {
        writeDataVariableDisplay(0x9030, "Kinetics");
        currentPageStart = 0;
        currentSearchMode = SEARCH_METHOD;
        startSearchByMethod(KINETICS);
        selectedPatientIndex = -1;

        uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
        uart_write_bytes(UART_PORT, (const char*)changePageTo4, sizeof(changePageTo4));

    } else if (len == sizeof(methfixtime) && matchCmd(data, methfixtime, sizeof(methfixtime))) {
        writeDataVariableDisplay(0x9030, "Fixed Time");
        currentPageStart = 0;
        currentSearchMode = SEARCH_METHOD;
        startSearchByMethod(FIXED_TIME);
        selectedPatientIndex = -1;

        uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
        uart_write_bytes(UART_PORT, (const char*)changePageTo4, sizeof(changePageTo4));

    } else if (len == sizeof(methAbsorbance) && matchCmd(data, methAbsorbance, sizeof(methAbsorbance))) {
        writeDataVariableDisplay(0x9030, "Absorbance");
        currentPageStart = 0;
        currentSearchMode = SEARCH_METHOD;
        startSearchByMethod(ABSORBANCE);
        selectedPatientIndex = -1;

        uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
        uart_write_bytes(UART_PORT, (const char*)changePageTo4, sizeof(changePageTo4));

    } else if (len == sizeof(patidApply) && matchCmd(data, patidApply, sizeof(patidApply))) {
        currentPageStart = 0;
        currentSearchMode = SEARCH_ID;
        if (strlen(idStr) > 0) {
            startSearchByIdPartial(idStr);
            selectedPatientIndex = -1;

            uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
            uart_write_bytes(UART_PORT, (const char*)changePageTo6, sizeof(changePageTo6));
        }

    } else if (len == sizeof(patusernameApply) && matchCmd(data, patusernameApply, sizeof(patusernameApply))) {
        currentPageStart = 0;
        currentSearchMode = SEARCH_USERNAME;
        if (strlen(usernameStr) > 0) {
            startSearchByUserName(usernameStr);
            selectedPatientIndex = -1;

            uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
            uart_write_bytes(UART_PORT, (const char*)changePageTo8, sizeof(changePageTo8));
        }

    } else if (len == sizeof(pattestnameApply) && matchCmd(data, pattestnameApply, sizeof(pattestnameApply))) {
        currentPageStart = 0;
        currentSearchMode = SEARCH_TESTNAME;
        if (strlen(testnameStr) > 0) {
            startSearchByTestName(testnameStr);
            selectedPatientIndex = -1;

            uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
            uart_write_bytes(UART_PORT, (const char*)changePageTo10, sizeof(changePageTo10));
        }

    } else if (len == sizeof(patdateApply) && matchCmd(data, patdateApply, sizeof(patdateApply))) {
        currentPageStart = 0;
        currentSearchMode = SEARCH_DATE;
        if ((strlen(dateStr1) > 0) && (strlen(dateStr2) > 0)) {
            startSearchByDate(dateStr1, dateStr2);
            selectedPatientIndex = -1;

            uart_write_bytes(UART_PORT, (const char*)resetvaricon, sizeof(resetvaricon));
            uart_write_bytes(UART_PORT, (const char*)changePageTo1, sizeof(changePageTo1));
        }
    } 
    else if(len == sizeof(row1click) && matchCmd(data, row1click, sizeof(row1click))){
        showRowInfo(0);
    } else if(len == sizeof(row2click) && matchCmd(data, row2click, sizeof(row2click))){
        showRowInfo(1);
    } else if(len == sizeof(row3click) && matchCmd(data, row3click, sizeof(row3click))){
        showRowInfo(2);
    } else if(len == sizeof(row4click) && matchCmd(data, row4click, sizeof(row4click))){
        showRowInfo(3);
    } 
    else if (len == sizeof(deletepatientindexdate) && matchCmd(data, deletepatientindexdate, sizeof(deletepatientindexdate))) {
        deletePatientAtIndex(selectedPatientIndex, changePageTo0, sizeof(changePageTo0));

    } else if (len == sizeof(deletepatientindexmethod) && matchCmd(data, deletepatientindexmethod, sizeof(deletepatientindexmethod))) {
        deletePatientAtIndex(selectedPatientIndex, changePageTo3, sizeof(changePageTo3));

    } else if (len == sizeof(deletepatientindexpatid) && matchCmd(data, deletepatientindexpatid, sizeof(deletepatientindexpatid))) {
        deletePatientAtIndex(selectedPatientIndex, changePageTo5, sizeof(changePageTo5));

    } else if (len == sizeof(deletepatientindexusername) && matchCmd(data, deletepatientindexusername, sizeof(deletepatientindexusername))) {
        deletePatientAtIndex(selectedPatientIndex, changePageTo7, sizeof(changePageTo7));

    } else if (len == sizeof(deletepatientindextestname) && matchCmd(data, deletepatientindextestname, sizeof(deletepatientindextestname))) {
        deletePatientAtIndex(selectedPatientIndex, changePageTo9, sizeof(changePageTo9));
    }
    else if (len == sizeof(viewpagedate) && matchCmd(data, viewpagedate, sizeof(viewpagedate))) {
        viewPatientPage(selectedPatientIndex, changePageTo2, sizeof(changePageTo2));
    } else if (len == sizeof(viewpagemethod) && matchCmd(data, viewpagemethod, sizeof(viewpagemethod))) {
        viewPatientPage(selectedPatientIndex, changePageTo13, sizeof(changePageTo13));
    } else if (len == sizeof(viewpagepatid) && matchCmd(data, viewpagepatid, sizeof(viewpagepatid))) {
        viewPatientPage(selectedPatientIndex, changePageTo14, sizeof(changePageTo14));
    } else if (len == sizeof(viewpageusername) && matchCmd(data, viewpageusername, sizeof(viewpageusername))) {
        viewPatientPage(selectedPatientIndex, changePageTo15, sizeof(changePageTo15));
    } else if (len == sizeof(viewpagetestname) && matchCmd(data, viewpagetestname, sizeof(viewpagetestname))) {
        viewPatientPage(selectedPatientIndex, changePageTo16, sizeof(changePageTo16));
    }
    else if (len >= 7 && data[3] == 0x83) {  
        uint16_t vpAddr = (data[4] << 8) | data[5];
        char *targetStr = NULL;
        size_t targetSize = 0;

        if (vpAddr == 0x9300) {           // ===== ID =====
            targetStr = idStr;
            targetSize = sizeof(idStr);
        } else if (vpAddr == 0x9320) {    // ===== USERNAME =====
            targetStr = usernameStr;
            targetSize = sizeof(usernameStr);
        } else if (vpAddr == 0x9330) {    // ===== TESTNAME =====
            targetStr = testnameStr;
            targetSize = sizeof(testnameStr);
        } else if (vpAddr == 0x9340) {    // ===== DATE STRING 1 =====
            targetStr = dateStr1;
            targetSize = sizeof(dateStr1);
        } else if (vpAddr == 0x9350) {    // ===== DATE STRING 2 =====
            targetStr = dateStr2;
            targetSize = sizeof(dateStr2);
        }

        if (targetStr != NULL) {
            int wordLen = data[6];
            int byteLen = wordLen * 2;

            if (byteLen > 0 && byteLen < targetSize) {
                memset(targetStr, 0, targetSize);
                memcpy(targetStr, &data[7], byteLen);

                // Stop at 0xFF or null padding
                for (int i = 0; i < byteLen; i++) {
                    if ((unsigned char)targetStr[i] == 0xFF || targetStr[i] == '\0') {
                        targetStr[i] = '\0';
                        break;
                    }
                }
            }
        }
    }


}


// Buffers
#define RX_BUFFER_SIZE 32
uint8_t rxBuffer[RX_BUFFER_SIZE];
int rxIndex = 0;


void handleIncomingCmd(uint8_t *data, int len); // your existing handler

void uartFrameTask(void *pvParameters) {
    uint8_t b;

    while (1) {
        int len = uart_read_bytes(UART_PORT, &b, 1, pdMS_TO_TICKS(10));
        if (len > 0) {
            // Debug: print every received byte in HEX
            ESP_LOGD(TAG, "0x%02X", b);

            // If buffer is empty → only accept 0x5A
            if (rxIndex == 0 && b != 0x5A) {
                continue; // wait for start
            }

            // If first byte was 0x5A, second must be 0xA5
            if (rxIndex == 1 && b != 0xA5) {
                rxIndex = 0; // reset and resync
                continue;
            }

            // Store in buffer
            if (rxIndex < RX_BUFFER_SIZE) {
                rxBuffer[rxIndex++] = b;
            }

            // Check if we have at least 3 bytes to know expected length
            if (rxIndex >= 3) {
                int expectedLen = rxBuffer[2];        // length byte
                int frameSize   = expectedLen + 3;    // header(2) + len(1) + data

                if (rxIndex == frameSize) {
                    // Full frame received
                    ESP_LOGD(TAG, "<-- Full Frame Received");

                    // Process full frame
                    handleIncomingCmd(rxBuffer, frameSize);

                    // Reset for next frame
                    rxIndex = 0;
                } else if (rxIndex > frameSize) {
                    // Overshoot → bad frame, reset and resync
                    rxIndex = 0;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // small delay to yield CPU
    }
}


extern "C" void app_main(void) {
    // UART config
    const uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // Install UART driver
    uart_driver_install(UART_PORT, 2048, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    ESP_LOGI("TAG", "Start");

    // Fill patients list
    patients.push_back({"1235", "Glucose",  "conca", "25-08-18", "09:15", ENDPOINT,  "Smith"});
    patients.push_back({"2028", "Cholester","conca", "12-02-21", "09:30", KINETICS,  "Adams"});
    patients.push_back({"3452", "Hemoglobi","conca", "07-01-20", "09:45", FIXED_TIME,"Brown"});
    patients.push_back({"4455", "Calcium",  "conca", "25-05-12", "10:00", ABSORBANCE,"White"});
    patients.push_back({"5455", "Creatinin","conca", "26-04-17", "10:15", ENDPOINT,  "Taylr"});
    patients.push_back({"6464", "Urea",     "conca", "13-06-18", "10:30", ENDPOINT,  "Green"});
    patients.push_back({"7454", "Bilirubi", "conca", "10-09-25", "10:45", FIXED_TIME,"Black"});
    patients.push_back({"8454", "Albumin",  "conca", "11-07-23", "11:00", ABSORBANCE,"Johno"});
    patients.push_back({"9454", "Lipase",   "conca", "21-03-22", "11:15", ENDPOINT,  "Evans"});
    patients.push_back({"1021", "Amylase",  "conca", "18-08-18", "11:30", ENDPOINT,  "Lewis"});

    // // Print to log
    // for (auto &p : patients) {
    //     ESP_LOGI(TAG, "ID: %s, Test: %s, Date: %s %s, Doctor: %s",
    //              p.id.c_str(), p.testName.c_str(),
    //              p.date.c_str(), p.time.c_str(),
    //              p.doctor.c_str());
    // }
    
    // Start the UART frame task
    xTaskCreate(uartFrameTask, "uartFrameTask", 4096, NULL, 10, NULL);

}


