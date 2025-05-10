
#include "MIServer.h"
#include <stdio.h>
#include <winsvc.h>
#include "MiConf.h"
#include "licenseproc.h"



int main(char* argv, int argc) {

    setting_init(1);

    //.
	ClaHTTPServerWrapper app;
	app.launch();
	return 0;
}