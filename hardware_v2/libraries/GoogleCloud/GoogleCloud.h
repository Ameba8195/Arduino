#ifndef GOOGLE_CLOUD_IOT_H
#define GOOGLE_CLOUD_IOT_H


class GoogleCloudClass {

public:
    GoogleCloudClass();    
    void setPrivatekey(unsigned char *privateKey);
    int gcConnect(char* project_id, char* registry_id, char* device_id);
    int publish();
    bool isconnected();


private:				
		unsigned char *_privateKeyBuff;

};

extern GoogleCloudClass gc_class;

#endif
