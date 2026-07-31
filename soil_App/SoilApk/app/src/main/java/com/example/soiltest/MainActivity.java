package com.example.soiltest;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
/*
put data to thingspeak
https://api.thingspeak.com/update?api_key=J4HV5ANMDLG5RGIQ&field1=30&field2=65&field3=25&field4=7&field5=60&field6=1.2

 */

public class MainActivity extends AppCompatActivity {

    private TextView nVal, pVal, kVal, phVal, moistVal, electVal;
    private TextView cropSug;

    private Handler handler = new Handler();
    private final int delay = 5000; // 5 seconds


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

        // Initialize TextViews
        nVal    = findViewById(R.id.nval);
        pVal    = findViewById(R.id.pval);
        kVal    = findViewById(R.id.kval);
        phVal   = findViewById(R.id.phval);
        moistVal = findViewById(R.id.moistval);
        electVal = findViewById(R.id.electval);
        cropSug  = findViewById(R.id.cropSug);

        // Start periodic task to fetch data
        startRepeatingTask();

    }
    private void startRepeatingTask() {
        handler.postDelayed(fetchTask, delay);
    }

    private final Runnable fetchTask = new Runnable() {
        @Override
        public void run() {
            fetchData();
            handler.postDelayed(this, delay); // Schedule next execution
        }
    };

    private void fetchData() {
        new FetchThingSpeakDataTask().execute();
    }

    private class FetchThingSpeakDataTask extends AsyncTask<Void, Void, String[]> {
        @Override
        protected String[] doInBackground(Void... voids) {
            try {
                // URL for fetching the data
                String url = "https://api.thingspeak.com/channels/2642662/feeds.json?api_key=ZO6MJVGNO0E4KIDD&results=1";

                // Create the request to the URL
                URL apiUrl = new URL(url);
                HttpURLConnection urlConnection = (HttpURLConnection) apiUrl.openConnection();
                try {
                    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                    StringBuilder stringBuilder = new StringBuilder();
                    String line;
                    while ((line = bufferedReader.readLine()) != null) {
                        stringBuilder.append(line).append("\n");
                    }
                    bufferedReader.close();
                    return parseJsonData(stringBuilder.toString());
                } finally {
                    urlConnection.disconnect();
                }
            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
        }

        @Override
        protected void onPostExecute(String[] result) {
            if (result != null) {
                // Update the TextViews with the fetched data
                nVal.setText(result[0] + " mg/Kg");
                pVal.setText(result[1] + " mg/Kg");
                kVal.setText(result[2] + " mg/Kg");
                phVal.setText(result[3]);
                moistVal.setText(result[4] + "%");
                electVal.setText(result[5]);

                // Suggest a crop based on these values
                suggestCrop(Double.parseDouble(result[0]), Double.parseDouble(result[1]), Double.parseDouble(result[2]),
                        Double.parseDouble(result[3]), Double.parseDouble(result[4]), Double.parseDouble(result[5]));
            }
        }

        private String[] parseJsonData(String jsonData) throws JSONException {
            JSONObject jsonObject = new JSONObject(jsonData);
            JSONArray feeds = jsonObject.getJSONArray("feeds");
            JSONObject latestData = feeds.getJSONObject(0);

            String nVal = latestData.getString("field1");
            String pVal = latestData.getString("field2");
            String kVal = latestData.getString("field3");
            String phVal = latestData.getString("field4");
            String moistVal = latestData.getString("field5");
            String electVal = latestData.getString("field6");

            return new String[]{nVal, pVal, kVal, phVal, moistVal, electVal};
        }

        private void suggestCrop(double n, double p, double k, double ph, double moisture, double electConductivity) {
            String suggestedCrop = "None";

            // Basic crop suggestion logic
            if (n > 50 && p > 30 && k > 20 && ph >= 6 && ph <= 7 && moisture > 40) {
                suggestedCrop = "Wheat";
            } else if (n > 40 && p > 20 && k > 10 && ph >= 5.5 && ph <= 6.5 && moisture > 50) {
                suggestedCrop = "Rice";
            } else if (ph > 7 && electConductivity < 1.5) {
                suggestedCrop = "Barley";
            } else if (moisture < 30) {
                suggestedCrop = "Millet";
            } else if (ph < 5.5) {
                suggestedCrop = "Corn";
            }

            // Update the crop suggestion TextView
            cropSug.setText("Suggested Crop: " + suggestedCrop);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacks(fetchTask); // Stop the handler when activity is destroyed
    }

}