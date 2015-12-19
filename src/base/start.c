#include "log.h"
#include "start.h"
#include "system_properties.h"

//Note: in Android system, it will be called when bionic lib is loaded.
//Every sub process of init should call this method in main() for mem share.
//We can't transplant bionic lib, so call it in main()
int __start_main() {
  LOG(INFO) << "start main()";
   __map_prop_area(); 

  return 0;
}
