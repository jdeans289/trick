
#include "trick/VariableServer.hh"

int Trick::VariableServer::shutdown() {
    listen_thread.cancel_thread() ;

    std::map < pthread_t , VariableServerThread * >::iterator it ;

    pthread_mutex_lock(&map_mutex) ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        (*it).second->cancel_thread() ;
    }
    pthread_mutex_unlock(&map_mutex) ;

    return 0 ;
}

