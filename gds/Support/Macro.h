#ifndef  __SUPPORT_MACRO_H__
#define  __SUPPORT_MACRO_H__


#ifndef PERIODICAL
#define PERIODICAL( i, x ) { static int count = 0; if(count==0) {(x);} count = (count+1) % i; }
#endif

#ifndef ONCE
#define ONCE( x ) { static int visited = 0; if(visited==0) {(x);} visited = 1; }
#endif

#ifndef numof
#define numof( x ) ( sizeof(x) / sizeof(x[0]) )
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE( p ) { if( p ) { delete p; p = NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete [] p; p = NULL; } }
#endif


#endif		/*  __SUPPORT_MACRO_H__  */
