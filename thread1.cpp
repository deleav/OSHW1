/* example.c*/
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>

void *thread( void *argu )
{
	int i;
	for(i=0;i<13;i++) {
		printf("This is a pthread.\n");
		sleep( 1 );
	}

	return NULL;
}

int main()
{
	pthread_t id;
	int i, ret;
	ret = pthread_create( &id, NULL, &thread, NULL );
	if ( ret != 0 ) {
		printf( "Create pthread error!\n" );
		_exit (1);
	}

	for ( i = 0 ; i < 13 ; i++ ) {
		printf( "This is the main process.\n" );
		sleep( 2 );
	}

	pthread_join( id, NULL );//等待線程（pthread）結束
	return 0;
}
/*
我們編譯此程式：
gcc example.c -lpthread -o example
*/
