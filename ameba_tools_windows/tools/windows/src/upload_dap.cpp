/*

Compile command:
	mingw32-g++.exe -o upload_dap.exe tools\windows\src\upload_dap.cpp -static

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

#define CHECK_DISK_RETRY 3

int retrieve_system_call_result (string cmd, vector<string> *lines) {

	int ret = 0;

	FILE *fin;
	stringstream ss;
	string line;
	const int bufsize = 256;
	char buf[bufsize];

	bool buffer_result = true;

	if (lines == NULL) {
		buffer_result = false;
	} else {
		lines->clear();
	}

	do {
		fin = popen(cmd.c_str(), "r");
		if (fin == NULL || errno != 0) {
			ret = -1;
			break;
		}

		while(fgets(buf, bufsize, fin) != NULL) {
			if (buffer_result) {
				ss << buf;
			}
		}

		// now ss contain the results. Dump into string vector
		if (buffer_result) {
			lines->clear();
			while(getline(ss, line)) {
				lines->push_back(line);
			}
		}
		pclose(fin);
	} while (0);

	return ret;
}

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

	int i , no_of_ameba_found = 0;
	bool mbed_disk_found = false, second_ameba_found = false;

	string cmd;
	vector<string> lines;
	vector<string>::iterator iter;

	stringstream ss;
	string disk_caption;
	string disk_volumename;
	string filepath;

	string filepath_ram_all;
	if (argc >= 2) {
		// the path of ram_all.bin is feed from arg
		filepath_ram_all.assign(argv[1]);
	} else {
		// assume ram_all.bin is in current path
		filepath_ram_all.assign("ram_all.bin");
	}

	do {
		cout << "Note: Please connect only one Ameba board at a time." <<endl;
		// 1. check if MBED disk driver exist
		for (i=0; i<CHECK_DISK_RETRY; i++) { //this for loop check 3 times until a disk with name "MBED" is found
			mbed_disk_found = false;
			cmd = "wmic logicaldisk get caption,volumename"; //command line
			retrieve_system_call_result(cmd, &lines); // as name suggested, this function retrieve the result of the command line

			for (iter = lines.begin() + 1; iter != lines.end() - 1; iter++) { //This for loop store the cmd result into disk_caption nad disk_volumename and compare result until "MBED" is found
				ss.clear();
				ss.str(*iter);
				ss >> disk_caption >> disk_volumename;

				if (disk_volumename.compare("MBED") == 0) {
					mbed_disk_found = true;
					no_of_ameba_found++;

					if (no_of_ameba_found >= 2) {
						second_ameba_found = true;
						break;
					}
					//break;  //once found, break from this for loop
				}
			}

			if (mbed_disk_found) {
				break; // if found, break from this for loop
			}
		}

		if (second_ameba_found) {
			cout << "ERR: Multiple Ameba boards found! Unplug and only have one connected to the host!" << endl;
			break;
		}
		if (!mbed_disk_found) {
			cout << "ERR: Cannot find ameba on mbed driver! Please re-plug Ameba." << endl;
			break;
		}

		// 2. check if MBED disk is accessable
		filepath = disk_caption + "\\mbed.htm";
		if (!isFileExist(filepath)) {
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
		cmd = "copy \"" + filepath_ram_all;
		cmd.append("\" ");
		cmd.append(disk_caption);
		cout << cmd << endl;
		system(cmd.c_str());

		cout << "upload finish" << endl;
	} while (0);

	return 0;
}
