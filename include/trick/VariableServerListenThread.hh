/*
    PURPOSE:
        (VariableServerListenThread)
*/

#ifndef VARIABLESERVERLISTENTHREAD_HH
#define VARIABLESERVERLISTENTHREAD_HH

#include <string>
#include <iostream>
#include "trick/ClientListener.hh"
#include "trick/SysThread.hh"
#include "trick/MulticastManager.hh"


namespace Trick {

/**
  This class runs the variable server listen loop.
  @author Alex Lin
 */
    class VariableServerListenThread : public Trick::SysThread {

        public:
            VariableServerListenThread() ;
            virtual ~VariableServerListenThread() ;

            const char * get_hostname() ;

            unsigned short get_port() ;
            void set_port(unsigned short in_port) ;

            std::string get_user_tag() ;
            const std::string& get_user_tag_ref() ;
            void set_user_tag(std::string in_tag) ;

            void set_source_address(const char * address) ;
            std::string get_source_address() ;

            bool get_broadcast() ;
            void set_broadcast(bool in_broadcast) ;

            virtual int init_listen_device() ;
            virtual int check_and_move_listen_device() ;

            virtual void * thread_body() ;

            int restart() ;

            // pause and restart listen thread during checkpoint reload
            void pause_listening() ;
            void restart_listening() ;

            void create_tcp_socket(const char * address, unsigned short in_port ) ;

            virtual void dump( std::ostream & oss = std::cout ) ;

        protected:
            void initializeMulticast();

            /** Requested variable server source address\n */
            std::string requested_source_address ;       /**<  trick_units(--) */

            /** Requested variable server port number.\n */
            unsigned short requested_port ;       /**<  trick_units(--) */

            /** User requested specific port number\n */
            bool user_requested_address ;  /**<  trick_units(--) */

            /** User defined unique tag to easily identify this variable server port.\n */
            std::string user_tag;          /**<  trick_units(--) */

            /** Turn on/off broadcasting of variable server port.\n */
            bool broadcast ;       /**<  trick_units(--) */

            /** The listen device */
            ClientListener listener;        /**<  trick_io(**) trick_units(--) */

            /* Multicast broadcaster */
            MulticastManager multicast;     /**<  trick_io(**) trick_units(--) */

            /** The mutex to stop accepting new connections during restart\n */
            pthread_mutex_t restart_pause ;     /**<  trick_io(**) */

    } ;

}

#endif

