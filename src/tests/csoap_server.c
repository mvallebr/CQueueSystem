#include <libcsoap/soap-env.h>
#include <libcsoap/soap-client.h>
#include <libcsoap/soap-router.h>
#include <libcsoap/soap-server.h>

SoapEnv* say_hello(SoapEnv *request) {

  SoapEnv *env;
  xmlNodePtr method, node;
  char *name;

  env = soap_env_new_with_response(request);

  method = soap_env_get_method(request);
  node = soap_xml_get_children(method);

  while (node) {
  
    name = (char*)xmlNodeListGetString(node->doc, 
		node->xmlChildrenNode, 1);
    if (!name) continue;

    soap_env_add_itemf(env,"xsd:string", "echo", "Hello '%s'", name);
    
    xmlFree((xmlChar*)name);

    node = soap_xml_get_next(node);
  }

  return env;
}

int main(int argc, char *argv[])
{
   SoapRouter *router;  
   if (!soap_server_init_args(argc, argv)) {
    return 0;
   }  
   router = soap_router_new();

   soap_router_register_service(router, say_hello, "primitive", "urn:examples");
   soap_server_register_router(router, "/csoapserver");

   soap_server_run();

   soap_router_free(router);
   soap_server_destroy();

  return 0;
}


