#include <stdlib.h>
#include "view/gl_applicationview.h"

#include "logger.h"

int main( int argc, char* argv[] ){
	glapp_init();
	if (argc >= 2) {
		glapp_loadState(argv[1]);
	}
	int code = glapp_run();
	exit(code);
}
