1. Generate a certificate for running ssl server:

	openssl genrsa -out ca.key 4096
	openssl req -new -x509 -days 365 -key ca.key -out ca.crt -sha256

2. Compile this example as below

	gcc -o amz_oauth_linux -c amz_oauth_linux.c -lssl -lcrypto

3. Then run it:

	./amz_oauth_linux

It'll show a link to authenticate, open it and follow the instruction.
Below is the sample log:

	$ ./amz_oauth_linux 
	Please enter device id: alexa_on_ameba
	Please enter device dsn: 123456
	Please enter client id: amzn1.application-oa2-client.001e8b07053e4154889cc92be9a41a37
	Please enter client secret: d0eed5ef5be75088cb92fd7e591ddfa9a0221163768b8fc46472fda8f7e4b2de
	Please enter allowed origins: https://localhost:3000
	Please enter allowed return urls: https://localhost:3000/authresponse
	Please enter server port: 3000

	Please open below link to authenticate:
	https://amazon.com/ap/oa?client_id=amzn1.application-oa2-client.001e8b07053e4154889cc92be9a41a37&response_type=code&redirect_uri=https://localhost:3000/authresponse&scope=alexa%3Aall&state=64a34e9b-b5f9-4258-89a7-2ec7e0d1f85a&scope_data=%7B%22alexa%3Aall%22%3A%7B%22productID%22%3A%22alexa_on_ameba%22%2C%22productInstanceAttributes%22%3A%7B%22deviceSerialNumber%22%3A%22123456%22%7D%7D%7D