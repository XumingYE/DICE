#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "../compress_cdc.h"

int main(int argc, char* argv[]) {

	DATA_IO f("/home/devuser/codebase/data/val_sql/sql_dump.tar");
	f.read_file(8192);

    return 0;

}
