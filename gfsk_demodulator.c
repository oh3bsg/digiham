#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUF_SIZE 1000
short buf[BUF_SIZE];

#define SAMPLES_PER_SYMBOL 10
#define VOLUME_RB_SIZE SAMPLES_PER_SYMBOL * 10

short volume_rb[VOLUME_RB_SIZE];
int volume_rb_pos = 0;

short min = 0, max = 0, centre = 0, umid = 0, lmid = 0;

void calibrate_audio() {
    int i;
    min = 0, max = 0;

    for (i = 0; i < VOLUME_RB_SIZE; i++) {
        if (volume_rb[i] < min) min = volume_rb[i];
        if (volume_rb[i] > max) max = volume_rb[i];
    }

    centre = (max + min) / 2;

    umid=((max-centre)*0.625)+centre;
    lmid=((min-centre)*0.625)+centre;        

    //fprintf(stderr, "min volume: %i max volume: %i centre: %i umid: %i lmid: %i\n", min, max, centre, umid, lmid);
}

void main() {
    int r = 0;
    int zcd[SAMPLES_PER_SYMBOL] = {0};
    int zcd_counter = 0;
    while ((r = fread(buf, 2, BUF_SIZE, stdin)) > 0) {
        //fprintf(stderr, "read %i bytes; symbols: ", r);

        int pos = 0;
        int output_size = r / SAMPLES_PER_SYMBOL;
        //fprintf(stderr, "output size: %i ", output_size);
        uint8_t output[output_size];
        while (pos * SAMPLES_PER_SYMBOL < r) {
            int i = 0, sum = 0;
            short last_value = 0;
            for (i = 0; i < SAMPLES_PER_SYMBOL; i++) {
                short value = buf[pos * SAMPLES_PER_SYMBOL + i];
                sum += value;
                volume_rb[volume_rb_pos + i] = buf[pos +i];
                //fprintf(stderr, "value: %i, last_value: %i : ", value, last_value);
                if (i > 0) {
                    if (last_value > centre ^ value > centre) {
                        zcd[i]++;
                    }
                }
                last_value = value;
            }
            //fprintf(stderr, "\n");
            zcd_counter ++;
            if (zcd_counter % 100 == 0) {
                int total_crossings = 0, low = 0, high = 0;
                for (i = 0; i < SAMPLES_PER_SYMBOL; i++) {
                    total_crossings += zcd[i];
                    if (i <= SAMPLES_PER_SYMBOL / 2) {
                        low += zcd[i];
                    } else {
                        high += zcd[i];
                    }
                }
                if (total_crossings > 0) {
                    fprintf(stderr, "zcd report: ");
                    for (i = 0; i < SAMPLES_PER_SYMBOL; i++) fprintf(stderr, "%3i ", zcd[i]);
                    double indication = ((double) high - low) / total_crossings;
                    fprintf(stderr, "indication: %s (%.2f%%)", indication >= 0 ? "positive" : "negative", indication * 100);
                    fprintf(stderr, "\n");
                }
                memset(zcd, 0, sizeof(zcd));
            }
            volume_rb_pos += SAMPLES_PER_SYMBOL;
            if (volume_rb_pos >= VOLUME_RB_SIZE) volume_rb_pos = 0;

            calibrate_audio();

            float average = sum / SAMPLES_PER_SYMBOL;

            if (average > centre) {
                if (average > umid) {
                    //fprintf(stderr, "1");
                    output[pos] = 1;
                } else {
                    //fprintf(stderr, "0");
                    output[pos] = 0;
                }
            } else {
                if (average < lmid) {
                    //fprintf(stderr, "3");
                    output[pos] = 3;
                } else {
                    //fprintf(stderr, "2");
                    output[pos] = 2;
                }
            }

            pos += 1;
        }
        //fprintf(stderr, "\n");

        fwrite(output, 1, output_size, stdout);
    }
}

