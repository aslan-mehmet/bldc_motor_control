#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

static char _temp_folder_name[] = "/tmp/serial_comm0000";

int remove_temp_folder(void)
{
	DIR *dir = opendir(_temp_folder_name);
	if (dir == NULL) {
		return -1;
	}

	size_t buf_size = sizeof(char) * (strlen(_temp_folder_name) + 500);
	char *buf = (char *) malloc(buf_size);
	
	getcwd(buf, buf_size);
	
	if (strcmp(buf, _temp_folder_name) != 0) { /* cwd not temp folder */
		closedir(dir);
		return -2;
	}
	
	struct dirent *ent = NULL;
	
	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(ent->d_name, ".") == 0) {
			continue;
		} else if (strcmp(ent->d_name, "..") == 0) {
			continue;
		}
		/* delete all the files in temp folder */
		remove(ent->d_name);
	}

	closedir(dir);
	/* dir must be empty */
	if (rmdir(_temp_folder_name) != 0) {
		return -3; 	/* means fail to delete */
	} 
	    
	return 0;
}

/* create temp folder and change directory */
int chdir_temp_folder(void)
{
	/* get pointer to number part */
	char *folder_number = _temp_folder_name + strlen("/tmp/serial_comm");

	struct stat buf;
	
	for (int i = 0; i < 9999; ++i) {
		snprintf(folder_number, sizeof(char) * 4, "%d", i);
		
		if (stat(_temp_folder_name, &buf)) { /* directory not exist */
			break;
		}
	}

	/* create, just allow user */
	if (mkdir(_temp_folder_name, S_IRWXU) != 0) {
		return -1;
	}
	
	if (chdir(_temp_folder_name) != 0) {
		return -2;
	}

	puts(_temp_folder_name);
	return 0;
}
