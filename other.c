#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "other.h"

#define KNOWN_COLS	4
#define KNOWN_VENDORS	1
#define KNOWN_DEVICES	2

static void privateFunction(void);
static void otherPrivateFunction(void);

int validId(char *id, char **list, int length) {
	int i;

	for(i = 0; i < length; i++) {
		if(!strcmp(id, list[i])) {
			return 1;
		}
	}

	return 0;
}

int pciDevices(void) {
	FILE *file;
	char *path = "/proc/bus/pci/devices";
	char *buff;
	int bufsize;
	int status;
	char *col;
	int i;
	char *col_names[KNOWN_COLS] = {"BusDevFunc", "Vendor Id + Device Id", "Interrupt Line", "Kernel driver in use"};
	char vendor_id[5] = {0};
	char device_id[5] = {0};
	char *vendors[] = {"11ab"};
	char *devices[] = {"2b38", "abcd"};

	printf("*** Opening file: %s\n", path);
	file = fopen(path, "r");
	
	buff = (char *)malloc(bufsize * sizeof(char));

	status = getline(&buff, (size_t *)&bufsize, file);
	while (status != -1) {
		i = 0;
		col = strtok(buff, "\t");
		//printf("\n");
		*vendor_id = '\0';
		*device_id = '\0';
  		while (col != NULL) {
			if(isalnum(*col) && i < KNOWN_COLS) {
				if(i == 1) {
					memcpy(vendor_id, col, 4);
					memcpy(device_id, col + 4, 4);
				}
				//printf("%s: %s\n", col_names[i], col);
				i++;
			}
			col = strtok(NULL, "\t");
		}
		//printf("[%s:%s - %d:%d]\n", vendor_id, device_id, validId(vendor_id, vendors, KNOWN_VENDORS), validId(device_id, devices, KNOWN_DEVICES));
		if (validId(vendor_id, vendors, KNOWN_VENDORS) && validId(device_id, devices, KNOWN_DEVICES)) {
			return 1;
		}
		status = getline(&buff, (size_t *)&bufsize, file);
	}

	fclose(file);

	return 0;
}

void verifyDeviceTree(void) {
	const char *fileName = "/boot/imx6-pixiepro.dtb";
	const char *string = "CODE PixiePro";

	if(!access(fileName, F_OK )) {
        	printf("The File %s\t was Found\n",fileName);
		if(!access(fileName, R_OK )) {
       			printf("The File %s\t can be read\n",fileName);
			if(matchString(fileName, string)) {
				printf("*** Valid file ***\n");
			} else {
				printf("*** Invalid file ***\tDevice not supported\n");
			}
		} else{
			printf("The File %s\t cannot be read\nDevice not supported\n",fileName);
    		}
    	} else {
        	printf("The File %s\t not Found\nDevice not supported\n",fileName);
    	}
}

int matchString(const char *filename, const char *string) {
	int fd;
	int file_size;
	int buff_size = strlen(string);
	int before, after;
	char buff[buff_size];
	int match = 0;
	
	fd=open(filename, O_RDONLY);
	
	printf("Verifying file ...\n");
	file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	printf("File size: %d bytes\n\n", file_size);
	
	while(read(fd, buff, 1) > 0) {
		//printf("[%d-%d] %c\n", before, after - 1, buff[0]);
		if(buff[0] == string[0]) {
			lseek(fd, -1, SEEK_CUR);
			before = lseek(fd, 0, SEEK_CUR);
			read(fd, buff, buff_size);
			after = lseek(fd, 0, SEEK_CUR);
			lseek(fd, -(buff_size - 1), SEEK_CUR);
			match = !strcmp(buff, string);
			if(match) {
				printf("[%d-%d] %s\n", before, after - 1, buff);

				return 1;
			}
		}
	}

	close(fd);

	return 0;
}

void publicFunction(void) {
	printf("Hola soy una función pública que vive en other.c\n");
	otherPrivateFunction();
}

static void privateFunction(void) {
	printf("Hola soy una función privada que vive en other.c\n");
}

static void otherPrivateFunction(void) {
	printf("Hola soy una función privada que vive en other.c que se invoca desde una función pública\n");
}