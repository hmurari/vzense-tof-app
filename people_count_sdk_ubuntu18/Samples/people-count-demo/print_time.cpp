#include <iostream>
#include <ctime>
#include <string.h>

using namespace std;

static char time_str[100];

static char *get_local_and_gm_time_str() {
	time_t tt;
	struct tm *local_ti;
	struct tm *utc_ti;

	time(&tt);
	local_ti = localtime(&tt);
	utc_ti = gmtime(&tt);

	char *local_time = asctime(local_ti);
	char *utc_time = asctime(utc_ti);
	int local_time_len = strlen(local_time);
	int utc_time_len = strlen(utc_time);

	strncpy(time_str, local_time, local_time_len);
	strncpy(&time_str[local_time_len - 1], " | ", 3);
        strncpy(&time_str[local_time_len + 2], utc_time, strlen(utc_time) - 1);

	return time_str;
}

static char *get_local_time_str() {
	time_t tt;
	struct tm *local_ti;

	time(&tt);
	local_ti = localtime(&tt);

	char *local_time = asctime(local_ti);
	int local_time_len = strlen(local_time);
	local_time[local_time_len - 1] = '\0';
	strncpy(time_str, local_time, local_time_len - 1);
	return time_str;
}

int main() {
	for (int idx = 0; idx < 10; idx++) {
		printf("%s\n", get_local_time_str());
	}
}

