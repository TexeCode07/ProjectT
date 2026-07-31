package com.example.flowapp;


import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;
import java.util.Map;

public class RecyclerAdapter extends RecyclerView.Adapter<RecyclerAdapter.ViewHolder> {

    private List<Map.Entry<String, Integer>> dataList;
    private OnPpmClickListener ppmClickListener;

    public RecyclerAdapter(List<Map.Entry<String, Integer>> dataList, OnPpmClickListener ppmClickListener) {
        this.dataList = dataList;
        this.ppmClickListener = ppmClickListener;

    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_layout, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        Map.Entry<String, Integer> item = dataList.get(position);
        holder.fl_value.setText(item.getValue() + " l/min");
        holder.fl_numb.setText( item.getKey()+ ".");



        if (item.getValue() > 0) {
            holder.fl_indic.setImageDrawable(ContextCompat.getDrawable(holder.itemView.getContext(), R.drawable.baseline_circle_24_g));
        } else {
            holder.fl_indic.setImageDrawable(ContextCompat.getDrawable(holder.itemView.getContext(), R.drawable.baseline_circle_24_r));
        }

        // Set click listener for fl_ppm
        holder.fl_ppm.setOnClickListener(v -> {
            if (ppmClickListener != null) {
                ppmClickListener.onPpmClick(position);
            }
        });
    }

    @Override
    public int getItemCount() {
        return dataList.size();
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        TextView fl_value;
        TextView fl_numb;
        ImageView fl_indic;
        ImageView fl_ppm;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            fl_value = itemView.findViewById(R.id.fl_value);
            fl_numb = itemView.findViewById(R.id.fl_numb);
            fl_indic = itemView.findViewById(R.id.fl_indic);
            fl_ppm = itemView.findViewById(R.id.fl_ppm);
        }
    }

    // Interface for ppm click listener
    public interface OnPpmClickListener {
        void onPpmClick(int position);
    }
}
