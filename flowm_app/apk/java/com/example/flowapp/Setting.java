package com.example.flowapp;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class Setting extends AppCompatActivity {

    ImageView eye1, eye2, eye3, eye4, eye5, eye6, eye7, eye8;

    // Initialize an array of seven boolean values with default values as false
    boolean[] visibilityStates = {true,true,true,true,true,true,true,true};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_setting);

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

        eye1 = findViewById(R.id.eye1);
        eye2 = findViewById(R.id.eye2);
        eye3 = findViewById(R.id.eye3);
        eye4 = findViewById(R.id.eye4);
        eye5 = findViewById(R.id.eye5);
        eye6 = findViewById(R.id.eye6);
        eye7 = findViewById(R.id.eye7);
        eye8 = findViewById(R.id.eye8);
        // Set the visibility of each drawable based on the visibility states
        updateDrawableVisibility();

        // Set onClick listeners for each ImageView
        setOnClickListeners();

    }

    private void updateDrawableVisibility() {
        // Array of ImageView references
        ImageView[] eyeViews = {eye1, eye2, eye3, eye4, eye5, eye6, eye7, eye8};

        // Loop through the visibilityStates array and set the corresponding visibility of each ImageView
        for (int i = 0; i < visibilityStates.length; i++) {
            if (visibilityStates[i]) {
                // If visibilityState is true, show the drawable (unhide)
                eyeViews[i].setImageResource(R.drawable.unhide);
            } else {
                // If visibilityState is false, hide the drawable
                eyeViews[i].setImageResource(R.drawable.hide);
            }
        }
    }

    private void setOnClickListeners() {
        // Array of ImageView references
        ImageView[] eyeViews = {eye1, eye2, eye3, eye4, eye5, eye6, eye7, eye8};

        // Loop through each ImageView and set an OnClickListener
        for (int i = 0; i < eyeViews.length; i++) {
            final int index = i; // Final index to use inside the OnClickListener

            eyeViews[i].setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    // Toggle the visibility state
                    visibilityStates[index] = !visibilityStates[index];

                    // Update the visibility of the drawable
                    if (visibilityStates[index]) {
                        eyeViews[index].setImageResource(R.drawable.unhide);
                    } else {
                        eyeViews[index].setImageResource(R.drawable.hide);
                    }

                    // Save the updated state to SharedPreferences
                    saveVisibilityStates();
                }
            });
        }
    }

    private void saveVisibilityStates() {
        // Get the SharedPreferences editor
        SharedPreferences sharedPreferences = getSharedPreferences("AppConfig", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        // Save each visibility state in SharedPreferences
        for (int i = 0; i < visibilityStates.length; i++) {
            editor.putBoolean("visibilityState_" + i, visibilityStates[i]);
        }

        // Commit the changes
        editor.apply();
    }
}