#include <stdio.h>
#include <stdlib.h>

int gt(double val, double threshold)
{
        if (val >= threshold) {
                return 1;
        }

        return 0;
}

int main(int argc, char **argv)
{
        char *line = NULL;
        char **lineptr = &line;
        size_t N = 0;
        size_t *n = &N;
        int seq;
        float ch[4];
        
        FILE *f, *fout;
        f = fopen(argv[1], "r");
        fout = fopen(argv[2], "w");
        
        getline(lineptr, n, f);
        getline(lineptr, n, f);

        while (getline(lineptr, n, f) >= 0) {
                sscanf(line, "%d,%f,%f,%f,%f,\n", &seq, ch, ch+1, ch+2, ch+3);
                
                double avg = (ch[0] + ch[1] + ch[2])/3.0;
                ch[0] = gt(ch[0], avg);
                ch[1] = gt(ch[1], avg);
                ch[2] = gt(ch[2], avg);

                fprintf(fout, "%d,%f,%f,%f,%f,\n", \
                        seq, ch[0], ch[1], ch[2], ch[3]);
                
        }

        fclose(f);
        fclose(fout);
        free(line);
        return 0;
}
