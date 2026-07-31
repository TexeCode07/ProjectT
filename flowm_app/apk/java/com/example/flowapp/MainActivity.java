package com.example.flowapp;

import android.app.Dialog;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.le.ScanResult;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

import android.Manifest;
import android.content.pm.PackageManager;
import android.widget.ProgressBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.welie.blessed.*;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.StandardCharsets;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.UUID;


public class MainActivity extends AppCompatActivity {


    private static final int REQUEST_PERMISSIONS = 1;

    // Define UUIDs for the service and characteristics
    public static final UUID SERVICE_UUID       = UUID.fromString("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    public static final UUID m_UUID             = UUID.fromString("beb5483e-36e1-4688-b7f5-ea07361b2697");
    public static final UUID p_UUID             = UUID.fromString("beb5483e-36e1-4688-b7f5-ea07361b2698");
    public static final UUID CALIBRATION_UUID   = UUID.fromString("beb5483e-36e1-4688-b7f5-ea07361b2699");

    String calibDataString;

//    ImageView   pump;
    FloatingActionButton fab;
    ProgressBar progressBar;
    RecyclerView recyclerView;
    private RecyclerAdapter adapter;

    private boolean pumpState = false;

    BluetoothCentralManager central;
    BluetoothPeripheral bluetoothPeripheral;

    boolean[] visibilityStates = {true,true,true,true,true,true,true,true};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        // Get the stored visibility states from SharedPreferences
        SharedPreferences sharedPreferences = getSharedPreferences("AppConfig", MODE_PRIVATE);

        // Retrieve each boolean value from SharedPreferences or set default value (false)
        for (int i = 0; i < visibilityStates.length; i++) {
            visibilityStates[i] = sharedPreferences.getBoolean("visibilityState_" + i, false); // false as default
        }

        progressBar = findViewById(R.id.progressBar);
//        pump        = findViewById(R.id.pump);
        recyclerView = findViewById(R.id.recyclerView);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        fab = findViewById(R.id.fab);
        requestPermissionsIfNecessary();
//
//        pump.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                togglePump();
//            }
//        });

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Create an intent to start the other activity
                Intent intent = new Intent(MainActivity.this, Setting.class);
                // Start the new activity
                startActivity(intent);
            }
        });


    }

//    private void togglePump() {
//        if (bluetoothPeripheral != null) {
//            // Retrieve the characteristic for the pump control
//            BluetoothGattCharacteristic characteristic = bluetoothPeripheral.getCharacteristic(SERVICE_UUID, p_UUID);
//
//            if (characteristic != null) {
//                pumpState = !pumpState;
//                if (pumpState){
//                    pump.setColorFilter(Color.GREEN);
//                }else{
//                    pump.setColorFilter(Color.RED);
//                }
//                byte[] command = new byte[]{(byte) (pumpState ? 1 : 0)};
//
//                // Check the properties of the characteristic to determine the appropriate write type
//                int properties = characteristic.getProperties();
//
//                try {
//                    if ((properties & BluetoothGattCharacteristic.PROPERTY_WRITE) != 0) {
//                        // Characteristic supports write operations
//                        Log.d("TAG", "Sent command to characteristic: " + Arrays.toString(command));
//                        bluetoothPeripheral.writeCharacteristic(characteristic, command, WriteType.WITHOUT_RESPONSE);
//
//                    } else if ((properties & BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE) != 0) {
//                        // Characteristic supports write without response
//                        Log.d("TAG", "Sent command to characteristic with no response: " + Arrays.toString(command));
//                        bluetoothPeripheral.writeCharacteristic(characteristic, command, WriteType.WITHOUT_RESPONSE);
//
//                    } else {
//                        Log.e("TAG", "Characteristic does not support write operations");
//                        Toast.makeText(getApplicationContext(), "Characteristic does not support write operations", Toast.LENGTH_SHORT).show();
//                    }
//                } catch (Exception e) {
//                    Log.e("TAG", "a:"+ e.getMessage());
//                    Toast.makeText(getApplicationContext(), "Failed to send command: " + e.getMessage(), Toast.LENGTH_LONG).show();
//                }
//
//            } else {
//                Log.e("TAG", "Characteristic not found for UUID: " + p_UUID);
//            }
//        } else {
//            Log.e("TAG", "Bluetooth peripheral is null");
//        }
//    }


    public void initializeBluetooth() {
        // Create BluetoothCentral and receive callbacks on the main thread
        central = new BluetoothCentralManager(getApplicationContext(), bluetoothCentralManagerCallback, new Handler(Looper.getMainLooper()));
        // Scan for peripherals with a specific service UUID
        central.scanForPeripheralsWithServices(new UUID[]{SERVICE_UUID});
        Log.d("TAG", "Bluetooth initialization started");
    }

    private final BluetoothCentralManagerCallback bluetoothCentralManagerCallback = new BluetoothCentralManagerCallback() {
        @Override
        public void onDiscoveredPeripheral(BluetoothPeripheral peripheral, ScanResult scanResult) {
            // Stop scanning and connect to the discovered peripheral
            central.stopScan();
            central.connectPeripheral(peripheral, peripheralCallback);
            Log.d("TAG", "Discovered peripheral: " + peripheral.getName());
        }

        @Override
        public void onDisconnectedPeripheral(@NonNull BluetoothPeripheral peripheral, @NonNull HciStatus status) {
            super.onDisconnectedPeripheral(peripheral, status);
//            pump.setColorFilter(Color.RED);
            initializeBluetooth();
        }
    };

    private final BluetoothPeripheralCallback peripheralCallback = new BluetoothPeripheralCallback() {
        @Override
        public void onServicesDiscovered(@NotNull BluetoothPeripheral peripheral) {

            // Request a higher MTU
            peripheral.requestMtu(185);
            // Request a new connection priority
            peripheral.requestConnectionPriority(ConnectionPriority.HIGH);
            // Set preferred PHY
            peripheral.setPreferredPhy(PhyType.LE_2M, PhyType.LE_2M, PhyOptions.S2);
            peripheral.readPhy();

            bluetoothPeripheral = peripheral;
            // Retrieve the service from the peripheral
            BluetoothGattService service = peripheral.getService(SERVICE_UUID);
            if (service == null) {
                Log.e("TAG", "Service " + SERVICE_UUID + " not found");
                return;
            }

            peripheral.setNotify(SERVICE_UUID, m_UUID, true);

            // When dialog opens, read the calibration data from the calibration UUID
            BluetoothGattCharacteristic calibrationCharacteristic = bluetoothPeripheral.getCharacteristic(SERVICE_UUID, CALIBRATION_UUID);

            if (calibrationCharacteristic != null) {
                peripheral.readCharacteristic(calibrationCharacteristic);
            }

        }


        @Override
        public void onCharacteristicUpdate(@NonNull BluetoothPeripheral peripheral, @NonNull byte[] value, @NonNull BluetoothGattCharacteristic characteristic, @NonNull GattStatus status) {
            super.onCharacteristicUpdate(peripheral, value, characteristic, status);

            progressBar.setVisibility(View.INVISIBLE);
//            pump.setVisibility(View.VISIBLE);

            Log.d("TAGM", "Characteristic update: " + Arrays.toString(value));
            if (status != GattStatus.SUCCESS) return;

            UUID characteristicUUID = characteristic.getUuid();

            // Handle characteristic update for the first characteristic
            if (characteristicUUID.equals(m_UUID)) {
                // Convert byte[] to String
                String jsonString = new String(value, StandardCharsets.UTF_8);

                // Parse JSON string
                try {
                    JSONObject jsonObject = new JSONObject(jsonString);

                    // Check characteristic UUID and update the corresponding UI fields
                    if (characteristicUUID.equals(m_UUID)) {
                        // Update fields with JSON values
                        // Create a list of key-value pairs
                        List<Map.Entry<String, Integer>> dataList = new ArrayList<>();
                        Iterator<String> keys = jsonObject.keys();
                        // Get the stored visibility states from SharedPreferences
                        SharedPreferences sharedPreferences = getSharedPreferences("AppConfig", MODE_PRIVATE);

                        // Retrieve each boolean value from SharedPreferences or set default value (false)
                        for (int i = 0; i < visibilityStates.length; i++) {
                            visibilityStates[i] = sharedPreferences.getBoolean("visibilityState_" + i, false); // false as default
                        }

                        int index = 0;
                        while (keys.hasNext()) {
                            String key = keys.next();
                            int valueInt = jsonObject.optInt(key, 0);
                            if(visibilityStates[index]){
                                dataList.add(new AbstractMap.SimpleEntry<>(key, valueInt));
                            }
                            index++;
                        }

                        runOnUiThread(() -> {
                            adapter = new RecyclerAdapter(dataList, position -> {
                                // Handle fl_ppm click event here
                                Log.d("TAG", "fl_ppm clicked at position: " + position);

                                showInputDialog(position);
                            });
                            recyclerView.setAdapter(adapter);
                            adapter.notifyDataSetChanged();
                        });

                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                    Log.e("TAGM", "JSON parsing error: " + e.getMessage());
                }
            } else if (characteristicUUID.equals(CALIBRATION_UUID)) {
                calibDataString = new String(value, StandardCharsets.UTF_8);
            }
        }

        @Override
        public void onNotificationStateUpdate(@NotNull BluetoothPeripheral peripheral, @NotNull BluetoothGattCharacteristic characteristic, @NotNull GattStatus status) {
            if (status == GattStatus.SUCCESS) {
                if (peripheral.isNotifying(characteristic)) {
                    Log.i("TAG", String.format("SUCCESS: Notify set to 'on' for %s", characteristic.getUuid()));
                } else {
                    Log.i("TAG", String.format("SUCCESS: Notify set to 'off' for %s", characteristic.getUuid()));
                }
            } else {
                Log.e("TAG", String.format("ERROR: Changing notification state failed for %s", characteristic.getUuid()));
            }
        }
    };

    private void requestPermissionsIfNecessary() {
        // List of permissions to request
        List<String> permissionsNeeded = new ArrayList<>();

        // Bluetooth permissions for Android 12 (API level 31) and higher
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                permissionsNeeded.add(Manifest.permission.BLUETOOTH_SCAN);
            }
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                permissionsNeeded.add(Manifest.permission.BLUETOOTH_CONNECT);
            }
        }

        // Location permission for BLE scanning
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            permissionsNeeded.add(Manifest.permission.ACCESS_FINE_LOCATION);
        }

        // Request permissions if not granted
        if (!permissionsNeeded.isEmpty()) {
            ActivityCompat.requestPermissions(this,
                    permissionsNeeded.toArray(new String[0]),
                    REQUEST_PERMISSIONS);
        } else {
            // Permissions are already granted, proceed to initialize Bluetooth
            initializeBluetooth();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_PERMISSIONS) {
            boolean allPermissionsGranted = true;

            // Check if all permissions are granted
            for (int result : grantResults) {
                if (result != PackageManager.PERMISSION_GRANTED) {
                    allPermissionsGranted = false;
                    break;
                }
            }

            if (allPermissionsGranted) {
                // Permissions granted, initialize Bluetooth
                initializeBluetooth();
            } else {
                // Handle the case where permissions are denied
                Toast.makeText(this, "Permissions denied. Bluetooth functionality is limited.", Toast.LENGTH_LONG).show();
            }
        }
    }

    private void showInputDialog(int position) {
        // Create a dialog
        Dialog dialog = new Dialog(this);
        dialog.setContentView(R.layout.ppm_box);

        // Get dialog elements
        EditText editTextValue = dialog.findViewById(R.id.editTextValue);
        Button btnSend = dialog.findViewById(R.id.btnSend);
        Button btnCancel = dialog.findViewById(R.id.btnCancel);


        // Parse the JSON data and get the corresponding factor based on the position
        try {
            JSONObject jsonObject = new JSONObject(calibDataString);

            // Map position to the corresponding factor key
            String factorKey = "factor" + (position + 1);

            // Get the value of the specific factor
            int factorValue = jsonObject.optInt(factorKey, 0);  // Default to 0 if the key doesn't exist

            // Set the value in the EditText
            editTextValue.setText(String.valueOf(factorValue));

            // Set onClickListener for the send button
            btnSend.setOnClickListener(v -> {
                // Get the new input value
                String inputValue = editTextValue.getText().toString().trim();

                try {
                    // Update the JSON object with the new value
                    jsonObject.put(factorKey, Integer.parseInt(inputValue));

                    // Convert the updated JSON object back to a string
                    calibDataString = jsonObject.toString();

                    // Log the updated JSON string
                    Log.d("TAG", "Updated JSON data: " + calibDataString);

                    // When dialog opens, read the calibration data from the calibration UUID
                    BluetoothGattCharacteristic calibrationCharacteristic = bluetoothPeripheral.getCharacteristic(SERVICE_UUID, CALIBRATION_UUID);

                    if (calibrationCharacteristic != null) {
                        // Convert the updated JSON string to bytes and write to the characteristic
                        byte[] calibrationDataBytes = calibDataString.getBytes(StandardCharsets.UTF_8);

                        // Write the characteristic with the new calibration data
                        bluetoothPeripheral.writeCharacteristic(calibrationCharacteristic, calibrationDataBytes, WriteType.WITHOUT_RESPONSE);

                    }

                } catch (JSONException e) {
                    e.printStackTrace();
                    Log.e("TAG", "Failed to update JSON data: " + e.getMessage());
                }

                dialog.dismiss(); // Close the dialog
            });

        } catch (JSONException e) {
            e.printStackTrace();
            Log.e("TAG", "Failed to parse JSON data: " + e.getMessage());
        }

        btnCancel.setOnClickListener(v -> {
            // Cancel button logic
            dialog.dismiss(); // Close the dialog
        });

        // Show the dialog
        dialog.show();
    }

}
