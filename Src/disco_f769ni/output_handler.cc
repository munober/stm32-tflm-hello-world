/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/micro/examples/hello_world/output_handler.h"

#include "stm32f769i_discovery_lcd.h"
#include "tensorflow/lite/micro/examples/hello_world/constants.h"

// Track whether the function has run at least once
bool initialized = false;
/*
// The LCD driver
LCD_DISCO_F746NG lcd;
*/
// The colors we'll draw
const uint32_t background_color = 0xFFF4B400;  // Yellow
const uint32_t foreground_color = 0xFFDB4437;  // Red
// The size of the dot we'll draw
const int dot_radius = 5;
// Size of the drawable area
int width;
int height;
// Midpoint of the y axis
int midpoint;
// Pixels per unit of x_value
int x_increment;


// Animates a dot across the screen to represent the current x and y values
void HandleOutput(tflite::ErrorReporter* error_reporter, float x_value,
                  float y_value) {
  static int x_pos_old, y_pos_old;
  // Do this only once
  if (!initialized) {
    // Calculate the drawable area to avoid drawing off the edges
    width = BSP_LCD_GetXSize() - (dot_radius * 2);
    height = BSP_LCD_GetYSize() - (dot_radius * 2);
    height = height/4;
    // Calculate the y axis midpoint
    midpoint = height / 2;
    // Calculate fractional pixels per unit of x_value
    x_increment = static_cast<float>(width) / kXrange;
    x_pos_old = 0;
    y_pos_old = midpoint;
    BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
    BSP_LCD_FillRect(0, 2*BSP_LCD_GetYSize()/8,
                     BSP_LCD_GetXSize(), 4*BSP_LCD_GetYSize()/8);
    initialized = true;
  }

  // Log the current X and Y values
  TF_LITE_REPORT_ERROR(error_reporter, "x_value: %f, y_value: %f\n", x_value,
                       y_value);

  // Calculate x position, ensuring the dot is not partially offscreen,
  // which causes artifacts and crashes
  int x_pos = dot_radius + static_cast<int>(x_value * x_increment);

  // Calculate y position, ensuring the dot is not partially offscreen
  int y_pos;
  if (y_value >= 0) {
    // Since the display's y runs from the top down, invert y_value
    y_pos = dot_radius + static_cast<int>(midpoint * (1.f - y_value));
  } else {
    // For any negative y_value, start drawing from the midpoint
    y_pos =
        dot_radius + midpoint + static_cast<int>(midpoint * (0.f - y_value));
  }
  // Draw the dot
  BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
  BSP_LCD_FillCircle(x_pos_old, BSP_LCD_GetYSize()/2-height/2+y_pos_old,
                     dot_radius);
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_FillCircle(x_pos, BSP_LCD_GetYSize()/2-height/2+y_pos, dot_radius);
  x_pos_old = x_pos;
  y_pos_old = y_pos;
}
