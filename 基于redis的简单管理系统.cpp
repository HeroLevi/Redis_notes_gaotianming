#include <iostream>
#include <hiredis/hiredis.h>
#include <list>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


unsigned int i = 0;

typedef struct
{
	char* userName;
	char* passWord;
	int userId;
}UserVo;
list<UserVo> uservo;

list<UserVo> QueryAll(redisContext* c)
{
	UserVo user;
	int key_num = 0;
	int nameflag = 1;
	int passflag = 1;
	char atmp[] = "userName";
	char btmp[] = "passWord";
	
	redisReply* rep = (redisReply*)redisCommand(c,"keys *");
	if ( rep->type == REDIS_REPLY_ERROR ) 
		printf( "Error: %s\n", rep->str );
	else if ( rep->type != REDIS_REPLY_ARRAY )   
		printf( "Unexpected type: %d\n", rep->type );
	else 
	{   
		int w;
		for(w=0;w<rep->elements;w++)
		{
			redisReply* r = (redisReply*)redisCommand(c,"HGETALL %d");
			if ( r->type == REDIS_REPLY_ERROR ) 
				printf( "Error: %s\n", r->str );
			else if ( r->type != REDIS_REPLY_ARRAY )   
				printf( "Unexpected type: %d\n", r->type );
			else
			{
				int q;
				for(q=0;q<r->elements;q++)
				{ 
					if(!strcmp(r->element[q]->str,atmp))
					{   
						nameflag = 0;
						continue;
					}
					if(!strcmp(r->element[q]->str,btmp))
					{
						passflag = 0;
						continue;
					}
					if(!nameflag)
					{   
						user.passWord = r->element[q]->str;
					}
					if(!passflag)
					{   
						user.userName = r->element[q]->str;
					}

				}
			}
		}
		uservo.push_back(user);
	}   

	return uservo;
}

list<UserVo> QueryByName(const char* name,redisContext* c)
{
	int key_num = 0;
	UserVo user;

	redisReply* rep = (redisReply*)redisCommand(c,"keys *");
	if ( rep->type == REDIS_REPLY_ERROR ) 
		printf( "Error: %s\n", rep->str );
	else if ( rep->type != REDIS_REPLY_ARRAY )   
		printf( "Unexpected type: %d\n", rep->type );
	else 
	{
		int w;
		for(w=0;w<rep->elements;w++)
		{
			redisReply* r = (redisReply*)redisCommand(c,"hget %d userName",*rep);
			if ( r->type == REDIS_REPLY_ERROR ) 
				printf( "Error: %s\n", r->str );
			else if ( r->type != REDIS_REPLY_ARRAY )   
				printf( "Unexpected type: %d\n", r->type );
			else
			{
				int q;
				for(q=0;q<r->elements;q++)
				{
					if(r->element[q]->str == name)
					{
						user.userName = r->element[q]->str;
						user.userId = atoi(rep->element[w]->str);
						user.passWord = r->element[q]->str;
						uservo.push_back(user);
					}
				}
			}
		}
	}
	return uservo;
}

UserVo QueryById(const int userId,redisContext* c)
{
	int nameflag = 1;
	int passflag = 1;
	char atmp[] = "userName";
	char btmp[] = "passWord";
	UserVo user;
	user.userId = userId;
	redisReply* rep = (redisReply*)redisCommand(c,"HGETALL %d",userId);

	cout<<rep->type<<endl;	
	if ( rep->type == REDIS_REPLY_ERROR ) 
		printf( "Error: %s\n", rep->str );
	else if ( rep->type != REDIS_REPLY_ARRAY )  
		printf( "Unexpected type: %d\n", rep->type );
	else 
	{
		int w;  
		cout<<rep->elements<<endl;
		for ( w=0; w<rep->elements; ++w )
		{
			if(!strcmp(rep->element[w]->str,atmp))
			{
				nameflag = 0;
				continue;
			}
			if(!strcmp(rep->element[w]->str,btmp))
			{
				passflag = 0;
				continue;
			}
			if(!nameflag)
			{
				user.passWord = rep->element[w]->str;
			}
			if(!passflag)
			{
				user.userName = rep->element[w]->str;
			}

		}
	}
	uservo.push_back(user);


	return user;
}

bool Insert(UserVo user,redisContext* c)
{
	redisReply* r = (redisReply*)redisCommand(c,"HMSET %d userName %s passWord %s",user.userId,user.userName,user.passWord);
	if(NULL == r)
	{
		return false;
	}
	return true;
}

bool Update(UserVo user,redisContext* c)
{
	cout<<"input userName and passWord"<<endl;
	char name[100],pass[100];
	cin>>name;
	cin>>pass;

	redisReply* r = (redisReply*)redisCommand(c,"HMSET %d userName %s passWord %s",user.userId,name,pass);
	if(NULL == r)
	{
		return false;
	}
	return true;
}

bool Delete(UserVo user,redisContext* c)
{
	redisReply* r = (redisReply*)redisCommand(c,"HDEL %d userName passWord",user.userId);
	if(NULL == r)
	{
		return false;
	}
	return true;
}

int main(void)
{
	redisContext* c = redisConnect((char*)"127.0.0.1",6379);
	if(c->err)
	{
		redisFree(c);
		cout<<"redis connect error!!!"<<endl;
		return 0;
	}

	UserVo user;
	user.userName = "fgnb";
	user.passWord = "123456789";
	user.userId = 100;

	UserVo test;

	Insert(user,c);

	test = QueryById(100,c);
	cout<<test.userId<<endl;
	QueryByName("fdnb",c);
	QueryAll(c);
	return 0;
}
