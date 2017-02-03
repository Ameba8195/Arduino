/*

compile command:
    g++ -m32 -o postbuild_img2_arduino_linux tools/linux/src/postbuild_img2_arduino_linux.cpp -static

*/

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {

    stringstream cmdss;
    string cmd, line;
    string path_tool;
    string path_arm_none_eabi_gcc;
    ifstream fin;

    bool has_sdram = false;
    string sram_start_st = "", sram_end_st = "", sdram_start_st = "", sdram_end_st = "";
    unsigned int sram_start = 0, sram_end = 0, sdram_start = 0, sdram_end = 0;

    size_t pos;

    // 0. change work folder
    chdir(argv[1]);

    // 1. copy elf application.axf to current folder
    cmdss.clear();
    cmdss << "cp " << argv[2] << " ./";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    // 2. remove previous files
    cmd = "rm -f application.map";
    cout << cmd << endl;
    system(cmd.c_str());

    cmd = "rm -f application.asm";
    cout << cmd << endl;
    system(cmd.c_str());

    cmd = "rm -f *.bin";
    cout << cmd << endl;
    system(cmd.c_str());

    // 3. generate information files
    path_arm_none_eabi_gcc.assign(argv[3]);

    cmdss.clear();
    cmdss << path_arm_none_eabi_gcc << "arm-none-eabi-nm application.axf | sort > application.map";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    cmdss.clear();
    cmdss << path_arm_none_eabi_gcc << "arm-none-eabi-objdump -d application.axf > application.asm";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    // 4. grep sram and sdram information
    fin.open("application.map");
    while ( getline(fin, line) ) {
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
    fin.close();

    if (sdram_start > 0 && sdram_end > 0) {
        has_sdram = true;
    }

    cout << "sram  " << sram_start_st << " ~ " << sram_end_st << endl;
    if (has_sdram) {
        cout << "sdram " << sdram_start_st << " ~ " << sdram_end_st << endl;
    }

    // 5. generate image 2 and image 3
    cmdss.clear();
    cmdss << path_arm_none_eabi_gcc << "arm-none-eabi-objcopy -j .image2.start.table -j .ram_image2.text -j .ram.data -Obinary ./application.axf ./ram_2.bin";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    if (has_sdram) {
        cmdss.clear();
        cmdss << path_arm_none_eabi_gcc << "arm-none-eabi-objcopy -j .image3 -j .ARM.exidx -j .sdr_data -Obinary ./application.axf ./sdram.bin";
        getline(cmdss, cmd);
        cout << cmd << endl;
        system(cmd.c_str());
    }

    // 6. fulfill header
    cmdss.clear();
    cmdss << "./tools/linux/pick " << sram_start << " " << sram_end << " ram_2.bin ram_2.p.bin body+reset_offset+sig";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    cmdss.clear();
    cmdss << "./tools/linux/pick " << sram_start << " " << sram_end << " ram_2.bin ram_2.ns.bin body+reset_offset";
    getline(cmdss, cmd);
    cout << cmd << endl;
    system(cmd.c_str());

    if (has_sdram) {
        cmdss.clear();
        cmdss << "./tools/linux/pick " << sdram_start << " " << sdram_end << " sdram.bin ram_3.p.bin body+reset_offset";
        getline(cmdss, cmd);
        cout << cmd << endl;
        system(cmd.c_str());
    }

    // 7. prepare image 1
    cmd = "cp bsp/image/ram_1.p.bin ./";
    cout << cmd << endl;
    system(cmd.c_str());

    cmd = "./tools/linux/padding 44k 0xFF ram_1.p.bin";
    cout << cmd << endl;
    system(cmd.c_str());

    // 8. generate ram_all.bin
    if (has_sdram) {
        cmd = "cat ram_1.p.bin ram_2.p.bin ram_3.p.bin > ram_all.bin";
        cout << cmd << endl;
        system(cmd.c_str());
        cmd = "cat ram_2.ns.bin ram_3.p.bin > ota.bin";
        cout << cmd << endl;
        system(cmd.c_str());
    } else {
        cmd = "cat ram_1.p.bin ram_2.p.bin > ram_all.bin";
        cout << cmd << endl;
        system(cmd.c_str());
        cmd = "cat ram_2.ns.bin > ota.bin";
        cout << cmd << endl;
        system(cmd.c_str());
    }

    // 9. add checksum
    cmd = "./tools/linux/checksum ota.bin";
    cout << cmd << endl;
    system(cmd.c_str());

    return 0;
}
