/**************************************************************************//**
 @File          apps.h

 @Description   AIOP Application Root file
                Contains the AIOP application initialization an 
                de-initialization setup

 @Cautions      None.
*//***************************************************************************/
/**************************************************************************//**
 @Group         aiopapp_init INIT (Initialization and Shutdown)

 @Description   At boot/compile time, each AIOP Application should register 
                it selves to the service layer. 
                The registration is done at compile time by providing 
                callbacks to a fixed name, static initialization and 
                de-initialization arrays.
                After loading the AIOP image to the AIOP memory, MC 
                enables the AIOP boot core code execution. As part of
                the AIOP initialization sequence, the AIOP service layer
                access the initialization array and calls the initialization 
                callbacks provided in the order they appear in the array. 
                Similar operation is executed at shutdown when the callbacks
                from the de-initialization array are called.   
                No arguments are provided to the initialization callbacks. 
                Initialization code is executed on a single core/cluster. 
  
 @{
*//***************************************************************************/ 
/**************************************************************************//**
 @Function      build_apps_array 

 @Description   Fixed name functoins that must be presented in the AIOP Application code.

 @Param[in]     apps - array with pointers to initialization and 
                       de-initialization functions provided by each 
                       AIOP application. 
 @Return        void 

 @Cautions      None
*//***************************************************************************/
void build_apps_array(struct sys_module_desc *apps);
/** @} */ /* end of aiopapp_init group */
