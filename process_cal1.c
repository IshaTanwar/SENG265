#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500
struct EVENT {
        char DTSTART[15];
        char DTEND[15];
        char UNTIL[15];
        char LOCATION[MAX_LINE_LEN];
        char SUMMARY[MAX_LINE_LEN];
        char INDICATE[MAX_LINE_LEN];
};
struct EVENT arr_event[MAX_EVENTS];

void dt_format(char*,const char*,const int);
void dt_increment(char*, const char*, const int);
char *am_pm(char*);

void print_events(int from_yy, int from_mm, int from_dd, int to_yy, int to_mm, int to_dd, int total_events, struct EVENT  *formatted_array){

    int i = 0;
    int first_event = 0;
    struct tm start, end;
    char *ds, *ts, *de, *te, dts[MAX_LINE_LEN], dte[MAX_LINE_LEN];
    char output[MAX_LINE_LEN];

    while(i < total_events) {
        strncpy(dts,formatted_array[i].DTSTART,MAX_LINE_LEN);
        strncpy(dte,formatted_array[i].DTEND,MAX_LINE_LEN);

        ds = strtok(dts, "T");
        ts = strtok(NULL, "\n");
        de = strtok(dte, "T");
        te = strtok(NULL, "\n");

        sscanf(ds, "%4d%2d%2d", &start.tm_year, &start.tm_mon, &start.tm_mday);
        sscanf(de, "%4d%2d%2d", &end.tm_year, &end.tm_mon, &end.tm_mday);

        if (start.tm_year >= from_yy && end.tm_year <= to_yy &&
        ((start.tm_mon >= from_mm && end.tm_mon < to_mm && start.tm_mday >= from_dd) || (start.tm_mon > from_mm && end.tm_mon <= to_mm && end.tm_mday <= to_dd) || (start.tm_mon > from_mm && end.tm_mon < to_mm) ||
        (start.tm_mon == from_mm && end.tm_mon == to_mm && (start.tm_mday >= from_dd && start.tm_mday <= to_dd)))) {

            sscanf(ts, "%2d%2d", &start.tm_hour, &start.tm_min);
            sscanf(te, "%2d%2d", &end.tm_hour, &end.tm_min);

            if (i == 0 || strncmp(formatted_array[i].DTSTART,formatted_array[i-1].DTSTART,9)) {
                if (first_event > 0) {
                    printf("\n");
                }

                strncpy(dts,formatted_array[i].DTSTART,MAX_LINE_LEN);
                strncpy(dte,formatted_array[i].DTEND,MAX_LINE_LEN);
                dt_format(output, dts, MAX_LINE_LEN);
                printf("%s\n", output);

                int dash = 0;
                while(dash < strlen(output)){
                                    printf("-");
                    dash++;
                }
                printf("\n");
            }

            if (start.tm_hour > 12) {
                start.tm_hour -= 12;
                printf("%2d:%02d ", start.tm_hour, start.tm_min);
            } else {
                printf("%2d:%02d ", start.tm_hour, start.tm_min);
            }

            printf("%s to ", am_pm(ts));

            if (end.tm_hour > 12) {
                end.tm_hour -= 12;
                printf("%2d:%02d ", end.tm_hour, end.tm_min);
            } else {
                printf("%2d:%02d ", end.tm_hour, end.tm_min);
            }

            printf("%s: ", am_pm(te));

            if (arr_event[i].SUMMARY != NULL) {
                printf("%s ", arr_event[i].SUMMARY);
            }

            printf("{{%s}}\n", arr_event[i].LOCATION);
            first_event++;

            }
        i++;
    }
}

char *am_pm(char *time) {
    struct tm temp_time;
    char *pm = "PM";
    char *am = "AM";

    sscanf(time, "%2d", &temp_time.tm_hour);
    if (temp_time.tm_hour == 12) {
        return pm;
    } else if (temp_time.tm_hour > 12) {
        return pm;
    } else {
        return am;
    }
}
void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;
    char      temp[5];

 memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)",
        localtime(&full_time));
}

int event_compare(const void *a, const void *b) {
    struct EVENT *aa = (struct EVENT*)a;
    struct EVENT *bb = (struct EVENT*)b;

    if (strcmp(aa->DTSTART,bb->DTSTART) < 0) {
        return -1;
    } else if (strcmp(aa->DTSTART,bb->DTSTART) == 0) {
        return 0;
    } else {
        return 1;
    }
}




void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time, *p_temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour,
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));
    strncpy(after + 16, before + 16, MAX_LINE_LEN - 16);
    after[MAX_LINE_LEN - 1] = '\0';
}

int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i;

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
          filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr,
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    /* Starting calling your own code from this point. */
            char line[MAX_LINE_LEN];
            char *property, *value;
            int updated = 0;
            int counter = updated;
            char start[MAX_LINE_LEN],end[MAX_LINE_LEN];



            /* parsing the file */
            FILE *fp_stream;
            fp_stream = fopen(filename, "r");

            while(fgets(line,132,fp_stream) != NULL) {
                property = strtok(line, ":");

                 if (!strcmp(property, "DTSTART")) {
                    value = strtok(NULL,"\n");
                    strncpy(arr_event[counter].DTSTART,value,strlen(value));

                } else if (!strcmp(property, "DTEND")) {
                    value = strtok(NULL,"\n");
                    strncpy(arr_event[counter].DTEND,value,strlen(value));

                } else if (!strcmp(property, "RRULE")) {
                    value = strtok(NULL,"=");
                    if (!strcmp(value,"FREQ")) {
                        value = strtok(NULL,";");
                        strncpy(arr_event[counter].INDICATE,value,strlen(value));
                        value = strtok(NULL,"=");
                    }
                    if (!strcmp(value,"UNTIL")) {
                        value = strtok(NULL,";");
                        strncpy(arr_event[counter].UNTIL,value,strlen(value));
                        value = strtok(NULL,"=");
                    }

                } else if (!strcmp(property, "LOCATION")) {
                    value = strtok(NULL,"\n");
                    if (value != NULL) {
                        strncpy(arr_event[counter].LOCATION,value,strlen(value));
                    }

                } else if (!strcmp(property, "SUMMARY")) {
                    value = strtok(NULL,"\n");
                    strncpy(arr_event[counter].SUMMARY,value,strlen(value));
                    if (!strcmp(arr_event[counter].INDICATE, "WEEKLY")) {
                        updated = counter;
                        while (strcmp(arr_event[updated].DTSTART, arr_event[counter].UNTIL)<0) {
                                dt_increment(start, arr_event[updated].DTSTART, 7);
                                dt_increment(end, arr_event[updated].DTEND, 7);
                                strncpy(arr_event[updated+1].DTSTART, start, strlen(start));
                                strncpy(arr_event[updated+1].DTEND, end, strlen(end));
                                strncpy(arr_event[updated].LOCATION, arr_event[counter].LOCATION, strlen(arr_event[counter].LOCATION));
                                strncpy(arr_event[updated].SUMMARY, arr_event[counter].SUMMARY, strlen(arr_event[counter].SUMMARY));
                                updated++;
                        }
                    }
                    if (updated > counter) {
                        counter = updated-1;
                    }
                    counter += 1;
                } // sum if
            }
            fclose(fp_stream);

            qsort(arr_event,counter,sizeof(struct EVENT),event_compare);
            print_events(from_y,from_m,from_d,to_y,to_m,to_d,counter,&arr_event[0]);

    exit(0);
}
