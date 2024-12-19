#include "esp_camera.h"
#include <WiFi.h>
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char* ssid = "ScumpiiFICCT";
const char* password = "12345678";

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  
  // Optimizaciones para fluidez
  config.frame_size = FRAMESIZE_VGA; // Reducido de SVGA a VGA para más velocidad
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST; // Siempre tomar el frame más reciente
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12; // Calidad más baja = más velocidad
  config.fb_count = 2;

  if(psramFound()){
    config.jpeg_quality = 15;  // Reducida de 10 a 15 para más velocidad
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  }

  // Inicializar cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  
  // Configuraciones optimizadas para streaming fluido
  s->set_framesize(s, FRAMESIZE_VGA);
  s->set_quality(s, 15); // Calidad más baja
  s->set_brightness(s, 0); // Neutral
  s->set_contrast(s, 0);   // Neutral
  s->set_saturation(s, 0); // Neutral
  s->set_special_effect(s, 0); // Sin efectos
  s->set_whitebal(s, 1);   // Habilitar balance de blancos
  s->set_awb_gain(s, 1);   // Habilitar ganancia AWB
  s->set_wb_mode(s, 0);    // Auto WB
  s->set_exposure_ctrl(s, 1); // Auto exposición
  s->set_aec2(s, 1);
  s->set_gain_ctrl(s, 1);
  s->set_bpc(s, 0);        // Deshabilitar corrección de píxeles defectuosos
  s->set_wpc(s, 0);        // Deshabilitar corrección de píxeles blancos
  s->set_raw_gma(s, 0);    // Deshabilitar corrección gamma
  s->set_lenc(s, 0);       // Deshabilitar corrección de lente
  s->set_hmirror(s, 0);    // Sin espejo
  s->set_vflip(s, 0);      // Sin volteo vertical
  s->set_dcw(s, 0);        // Deshabilitar DCW
  s->set_colorbar(s, 0);   // Sin barra de colores

  // Iniciar WiFi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false); // Deshabilitar sleep mode para mejor rendimiento

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream' to connect");
}

void loop() {
  // Loop vacío para mejor rendimiento
}