/*
 * This sketch shows how to get current OS free heap size
 *
 * In FreeRTOS, there is a heap for memory allocale and creating thread.
 * If heap size is too low, then some function may behave abnormal from the failure of memory allocation.
 */

void setup() {
  char *buf;

  Serial.print("Dynamic memory size: ");
  Serial.println(os_get_free_heap_size());
  Serial.println();

  buf = (char *) malloc ( 1000 );
  Serial.println("Allocate 1000 bytes");
  Serial.print("Dynamic memory size: ");
  Serial.println(os_get_free_heap_size());
  Serial.println();

  free(buf);
  Serial.println("Free 1000 bytes");
  Serial.print("Dynamic memory size: ");
  Serial.println(os_get_free_heap_size());
}

void loop() {
	delay(1000);
}