#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "xxxxxxx";
const char* password = "xxxxxx";

// Server URL for uploading images
const char* serverUrl = "http://192.168.235.17:5000/upload";

// Camera model pins configuration
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

int captureCount = 0;  // Counter for the number of captures
const int maxCaptures = 6;  // Maximum number of images to capture
bool firstImageIgnored = false;  // Flag to indicate if the first image is ignored

void setup() {
  Serial.begin(115200);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Adjust camera settings for better exposure
  sensor_t* s = esp_camera_sensor_get();
  s->set_brightness(s, 1);  // 0 is the default
  s->set_contrast(s, 2);
  s->set_saturation(s, 2);
  s->set_gainceiling(s, GAINCEILING_8X);  // Increase the gain ceiling
  s->set_exposure_ctrl(s, 1);  // Enable exposure control
  s->set_whitebal(s, 1);  // Enable white balance
  s->set_awb_gain(s, 1);  // Enable auto white balance gain
  s->set_wb_mode(s, 0);  // Auto white balance mode
  s->set_aec2(s, 0);  // Disable AEC algorithm
  s->set_ae_level(s, 0);  // Auto exposure level
  s->set_aec_value(s, 300);  // Manual exposure control value

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Discard the first image to avoid sending the black frame
  camera_fb_t* fb = esp_camera_fb_get();
  if (fb) {
    Serial.println("First image captured and discarded.");
    esp_camera_fb_return(fb);  // Discard the first frame
    firstImageIgnored = true;  // Mark the first image as ignored
  }
}

void loop() {
  if (captureCount < maxCaptures && firstImageIgnored) {
    // Capture an image
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Send image to server
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "image/jpeg");
    int httpResponseCode = http.POST(fb->buf, fb->len);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Response from server: ");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Cleanup
    esp_camera_fb_return(fb);
    captureCount++;  // Increment the capture counter
    delay(10000);  // Delay between captures
  } else if (captureCount >= maxCaptures) {
    Serial.println("Maximum captures reached. Stopping.");
    while (true) {
      // Halt further operations after reaching the limit
      delay(10000);
    }
  }
}
