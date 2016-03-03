#include "message.h"

void login_msg(message *segment){

	(*segment).code = 200;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "FROM:%s", General_Name);

}
