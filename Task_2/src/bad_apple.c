#include <ao/ao.h>
#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../incl/builtin.h"

int main(int argc, char const *argv[]) {
  int pid = fork();
  if (pid == 0) {
    mpg123_handle *audio;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int driver;
    ao_device *dev;

    ao_sample_format format;
    int channels, encoding;
    long rate;

    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    audio = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(audio);
    buffer = (unsigned char *)malloc(buffer_size * sizeof(unsigned char));

    mpg123_open(audio, "../src/badapple.mp3");
    mpg123_getformat(audio, &rate, &channels, &encoding);

    format.bits = mpg123_encsize(encoding) * 8;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);

    while (mpg123_read(audio, buffer, buffer_size, &done) == MPG123_OK)
      ao_play(dev, buffer, done);

    free(buffer);
    ao_close(dev);
    mpg123_close(audio);
    mpg123_delete(audio);
    mpg123_exit();
    ao_shutdown();
  } else {
    FILE *fp = fopen("../src/badapple.txt", "r");
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    printf("%d\n", file_size);
    char *full_animation = (char *)malloc(file_size * sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(full_animation, file_size, sizeof(char), fp);
    const char *delim = "frame_mark";
    char *frame = strtok(full_animation, delim);
    do {
      muttClear();
      printf("%s", frame);
      usleep(114500);
    } while ((frame = strtok(NULL, delim)) != NULL);
  }
  return 0;
}