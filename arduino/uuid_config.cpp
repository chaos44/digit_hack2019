#include <bluefruit.h>
#include <Bluefruit_FileIO.h>
#include "debug_print.h"


// blesv_user_uuid

/*********************************************************************************
  Internal config file
*********************************************************************************/
#define UUID_FILENAME "/uuidconfig.txt"
#define UUID_SIZE 16

File file(InternalFS);

void configFileInit() {
    InternalFS.begin();
}
/*********************************************************************************
  UUID Configure data
*********************************************************************************/
void configFileWrite(uint8_t binUuid[]) {
  int i = 0;
  if (file.open(UUID_FILENAME, FILE_WRITE)) {
    file.seek(0);

    for (i = 0; i < UUID_SIZE; i++) {
      file.write(binUuid[i]);
    }
    file.close();
  } else {
    debugPrint("[UUID]Write UUID : Failed!");
  }
}

void configFileRead(uint8_t binUuid[]) {
  file.open(UUID_FILENAME, FILE_READ);
  file.read(binUuid, UUID_SIZE);
  file.close();
}

int configFileExist() {
  file.open(UUID_FILENAME, FILE_READ);
  if (!file) {
    file.close();
    return -1;
  }
  file.close();
  return 0;
}

int compareUuid(uint8_t uuid1[], uint8_t uuid2[]) {
  for (int i = 0; i < UUID_SIZE; i++) {
    if (uuid1[i] != uuid2[i]) {
      return -1;
    }
  }
  return 0;
}
