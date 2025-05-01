#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main() {

	int SMALLER = 0;
	int BIGGER = 0;
	int TEMP = 0;
	bool BJ = false;

    printf("Please give a value for BIGGER: ");
	scanf("%d", &BIGGER); 

    printf("Please give a value for SMALLER: ");
	scanf("%d", &SMALLER); 

	BIGGER = BIGGER + 1 ; 
	BIGGER = ( BIGGER - 1 ) ; 
	BIGGER = BIGGER * SMALLER ; 
	BJ = true ; 
	BIGGER = BIGGER % SMALLER ; 

	if ( ( SMALLER > BIGGER + ( 3 + 2 ) ) == true ) { 
		TEMP = SMALLER ; 
		SMALLER = BIGGER ; 
		BIGGER = TEMP ; 
		if ( 3 + 4 ) { 
			TEMP = 4 ; 
		} else {
			TEMP = 3 ; 
		}
	} else {
		TEMP = BIGGER ; 
	}
	
	while ( SMALLER > 0 ) { 
		BIGGER = BIGGER * SMALLER ; 
		BJ = true ; 
		if ( ( SMALLER > BIGGER ) > 3 ) { 
			TEMP = SMALLER ; 
			SMALLER = BIGGER ; 
			BIGGER = TEMP ; 
		}
        SMALLER = 0 ;
	}
    printf("BIGGER: ");
	printf("%d", BIGGER );
    printf("\nBye\n");
}
