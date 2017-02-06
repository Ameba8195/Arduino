/*

Compile command:
	mingw32-g++.exe -o postbuild_img2_arduino_windows.exe tools\windows\src\postbuild_img2_arduino_windows.cpp -static

*/

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <algorithm>

using namespace std;

void replaceAll( string& source, const string& from, const string& to )
{
    string newString;
    newString.reserve( source.length() );  // avoids a few memory allocations

    string::size_type lastPos = 0;
    string::size_type findPos;

    while( string::npos != ( findPos = source.find( from, lastPos )))
    {
        newString.append( source, lastPos, findPos - lastPos );
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr( lastPos );

    source.swap( newString );
}

int main(int argc, char *argv[]) {

    stringstream cmdss;
    string cmd, line;
	vector<string> lines;
	vector<string>::iterator iter;
    string path_tool;
    string path_arm_none_eabi_gcc;
    ifstream fin;
    ofstream fout;

    bool has_sdram = false;
    string sram_start_st = "", sram_end_st = "", sdram_start_st = "", sdram_end_st = "";
    unsigned int sram_start = 0, sram_end = 0, sdram_start = 0, sdram_end = 0;

    size_t pos;

    // 0. change work folder
    chdir(argv[1]);

    // 1. copy elf application.axf to current folder
    cmdss.clear();
    cmdss << "xcopy /y " << argv[2] << " .\\";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    // 2. remove previous files
    cmd = "if exist application.map del application.map";
    cout << cmd << endl;
    system(cmd.c_str());

    cmd = "if exist application.asm del application.asm";
    cout << cmd << endl;
    system(cmd.c_str());

    cmd = "if exist *.bin del *.bin";
    cout << cmd << endl;
    system(cmd.c_str());

    // 3. generate information files
    path_arm_none_eabi_gcc.assign(argv[3]);
    replaceAll(path_arm_none_eabi_gcc, "/", "\\");

    cmdss.clear();
    cmdss << "\"" <<path_arm_none_eabi_gcc << "arm-none-eabi-nm.exe\" application.axf > application.map";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    fin.open("application.map");
    while( getline(fin, line) ) {
        lines.push_back(line);
    }
    fin.close();
    sort(lines.begin(), lines.end());

    cmdss.clear();
    cmdss << "\"" <<path_arm_none_eabi_gcc << "arm-none-eabi-objdump.exe\" -d application.axf > application.asm";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    // 4. grep sram and sdram information
	fout.open("application.map");
    for (iter = lines.begin(); iter != lines.end(); ++iter) {
		fout << *iter << endl;
        line = *iter;
        pos = line.find("__ram_image2_text_start__");
        if ( pos != string::npos ) {
            sram_start_st = line.substr(0, pos-3);
            sram_start = strtol(sram_start_st.c_str(), NULL, 16);
        }
        pos = line.find("__ram_image2_text_end__");
        if ( pos != string::npos ) {
            sram_end_st = line.substr(0, pos-3);
            sram_end = strtol(sram_end_st.c_str(), NULL, 16);
        }
        pos = line.find("__sdram_data_start__");
        if ( pos != string::npos ) {
            sdram_start_st = line.substr(0, pos-3);
            sdram_start = strtol(sdram_start_st.c_str(), NULL, 16);
        }
        pos = line.find("__sdram_data_end__");
        if ( pos != string::npos ) {
            sdram_end_st = line.substr(0, pos-3);
            sdram_end = strtol(sdram_end_st.c_str(), NULL, 16);
        }
    }
	fout.close();

    if (sdram_start > 0 && sdram_end > 0) {
        has_sdram = true;
    }

    cout << "sram  " << sram_start_st << " ~ " << sram_end_st << endl;
    if (has_sdram) {
        cout << "sdram " << sdram_start_st << " ~ " << sdram_end_st << endl;
    }

    // 5. generate image 2 and image 3
    cmdss.clear();
    cmdss << "\"" <<path_arm_none_eabi_gcc << "arm-none-eabi-objcopy.exe\" -j .image2.start.table -j .ram_image2.text -j .ram.data -Obinary .\\application.axf .\\ram_2.bin";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    if (has_sdram) {
        cmdss.clear();
        cmdss << "\"" << path_arm_none_eabi_gcc << "arm-none-eabi-objcopy.exe\" -j .image3 -j .ARM.exidx -j .sdr_data -Obinary .\\application.axf .\\sdram.bin";
        getline(cmdss, cmd);
        cout << cmd << endl;
        system(cmd.c_str());
    }

    // 6. fulfill header
    cmdss.clear();
    cmdss << ".\\tools\\windows\\pick.exe " << sram_start << " " << sram_end << " ram_2.bin ram_2.p.bin body+reset_offset+sig";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    cmdss.clear();
    cmdss << ".\\tools\\windows\\pick.exe " << sram_start << " " << sram_end << " ram_2.bin ram_2.ns.bin body+reset_offset";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    if (has_sdram) {
        cmdss.clear();
        cmdss << ".\\tools\\windows\\pick.exe " << sdram_start << " " << sdram_end << " sdram.bin ram_3.p.bin body+reset_offset";
        getline(cmdss, cmd);
        cout << cmd << endl;
        system(cmd.c_str());
    }

    // 7. prepare image 1
    cmd = "copy bsp\\image\\ram_1.p.bin .\\";
    cout << cmd << endl;
    system(cmd.c_str());

    cmd = ".\\tools\\windows\\padding.exe 44k 0xFF ram_1.p.bin";
    cout << cmd << endl;
    system(cmd.c_str());

    // 8. generate ram_all.bin
    if (has_sdram) {
        cmd = "copy /b ram_1.p.bin+ram_2.p.bin+ram_3.p.bin ram_all.bin";
        cout << cmd << endl;
        system(cmd.c_str());
        cmd = "copy /b ram_2.ns.bin+ram_3.p.bin ota.bin";
        cout << cmd << endl;
        system(cmd.c_str());
    } else {
        cmd = "copy /b ram_1.p.bin+ram_2.p.bin ram_all.bin";
        cout << cmd << endl;
        system(cmd.c_str());
        cmd = "copy /b ram_2.ns.bin ota.bin";
        cout << cmd << endl;
        system(cmd.c_str());
    }

    // 9. add checksum
    cmd = ".\\tools\\windows\\checksum.exe ota.bin";
    cout << cmd << endl;
    system(cmd.c_str());

    return 0;
}
