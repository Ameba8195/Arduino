/*

Compile command:
        g++ -o upload_dap_macosx tools/macosx/src/upload_dap_macosx.cpp

*/

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <sys/stat.h>

using namespace std;

bool isFileExist(string path) {
	bool ret = false;
	struct stat info;

	do {
		if (stat(path.c_str(), &info) != 0) {
			break;
		}

		ret = true;
	} while (0);

	return ret;
}

int main(int argc, char *argv[]) {

	int i;
	string cmd;
	vector<string> lines;
	vector<string>::iterator iter;

	stringstream ss;
	string disk_caption;
	string disk_volumename;
	string filepath1, filepath2;

	string filepath_ram_all;
	if (argc >= 2) {
		// the path of ram_all.bin is feed from arg
		filepath_ram_all.assign(argv[1]);
	} else {
		// assume ram_all.bin is in current path
		filepath_ram_all.assign("ram_all.bin");
	}

	do {
		// 2. check if MBED disk is accessable
		cout << "Note: Please connect only one Ameba board at a time" << endl;

		filepath1 = "/Volumes/MBED/mbed.htm";
		filepath2 = "/Volumes/MBED 1/mbed.htm";//the second Ameba

		if (isFileExist(filepath1)) {
			if (isFileExist(filepath2)) {
				cout << "ERR: Multiple Ameba detected, pls unplug the second one!" << endl;
				break;
			}
		} else {
			cout << "ERR: Cannot access mbed driver!" << endl;
			break;
		}

		// 3. check if ram_all.bin exist
		if (!isFileExist(filepath_ram_all)) {
			cout << "ERR: Cannot find ram_all.bin!" << endl;
			break;
		}

		// 4. copy ram_all.bin into mbed device
		cout << "uploading..." << endl;
		cmd = "cp \"" + filepath_ram_all;
		cmd.append("\" /Volumes/MBED/;sync");
		cout << cmd << endl;
		system(cmd.c_str());

		cout << "upload finish" << endl;
	} while (0);

	return 0;
}
