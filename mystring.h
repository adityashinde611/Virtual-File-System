int mystrlen(char str[])
{
	int i_counter = 0;
	while( *str != '\0')
	{
		i_counter ++;
		*str ++;
	}
	return i_counter;
}

char* mystrcpy(char *dest,const char *source)
{
	while(*source != '\0')
	{
		*dest = *source;
		*dest ++;
		*source ++;
	}
	*dest = '\0';
	return dest;
}

char* mystrncpy(char *dest, const char *source,int n)
{
	int i_counter = 0;
	while(*source != '\0' && i_counter < n)
	{
		*dest = *source;
		i_counter++;
		dest ++;
		source ++;
	}
	*dest = '\0';
	return dest;
}

char* mystrcat(char *dest,const char *source)
{
	while(*dest != '\0')
	{
		dest ++;
	}
	while(*source != '\0')
	{
		*dest = *source;
		dest ++;
		source ++;
	}
	*dest = '\0';
	return dest;
}

char* mystrncat(char *dest,const char *source,int n)
{
	int i_counter = 0;
	while(*dest != '\0')
	{
		dest ++;
	}
	while(*source != '\0' && i_counter < n)
	{
		*dest = *source;
		dest ++;
		source ++;
		i_counter ++;
	}
	*dest = '\0';
	return dest;
}

int mystrcmp(const char *str1,const char *str2)
{
	while(*str1 != '\0' || *str2 != '\0')
	{
		if(*str1 != *str2)
			break;
		str1 ++;
		str2 ++;
	}
	if(*str1 == *str2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int mystrncmp(const char *str1,const char *str2,int n)
{
	int i_counter = 0;
	while((*str1 != '\0' || *str2 != '\0') && i_counter < n)
	{
		if(*str1 != *str2)
			break;
		str1 ++;
		str2 ++;
		i_counter ++;
	}
	if(*str1 == *str2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int mystrcmpi(const char *str1,const char *str2)
{
	while(*str1 != '\0' || *str2 != '\0')
	{
		if(*str1 == *str2 || *str1 - *str2 == -32 || *str1 - *str2 == 32)
		{
			str1 ++;
			str2 ++;
		}
		else
			break;
	}
	if(*str1 == *str2)
		return 0;
	else
		return 1;
}

int mystrncmpi(const char* str1,const char* str2,int n)
{
	int i_counter = 0;
	while((*str1 != '\0' || *str2 != '\0')&&(i_counter < n))
	{
		if(*str1 == *str2 || *str1 - *str2 == 32 ||*str1 - *str2 == -32)
		{
			str1 ++;
			str2 ++;
			i_counter ++;
		}
		else
		{
			break;
		}
	}		
	if(i_counter == n)
		return 0;
	else
		return 1;
}

char * mystrrev(char *str)
{
	char *pstart = str;
	char *pend = str;
	char temp = '\0';
	
	while(*pend != '\0')
	{
		pend ++;
	}
	pend--;
	while(pstart < pend)
	{
		temp = *pstart;
		*pstart = *pend;
		*pend = temp;
		pstart ++;
		pend --;
	}
	return str;
}

char * mystrlwr(char *str)
{
	while(*str != '\0')
	{
		if(*str >= 'A' && *str <= 'Z')
		{
			*str = *str + 32;
		}
		str ++;
	}
	*str = '\0';
	return str;
}

char * mystrupr(char *str)
{
	while(*str != '\0')
	{
		if(*str >= 'a' && *str <= 'z')
		{
			*str = *str - 32;
		}
		str ++;
	}
	*str = '\0';
	return str;
}

char *mystrset(char *str,char ch)
{
	while(*str != '\0')
	{
		*str = ch;
		str ++;
	}
	*str = '\0';
	return str;
}

char *mystrnset(char *str,char ch,int n)
{
	int i_counter = 0;
	while((*str != '\0')&& (i_counter < n))
	{
		*str = ch;
		str ++;
		i_counter ++;
	}
	*str = '\0';
	return str;
}

char *mystrchr(const char *str,char ch)
{
	while(*str != '\0')
	{
		if(*str == ch)
			return (char*)str;
		
		str ++;
	}
	return NULL;
}

char* mystrrchr(const char* str,char ch)
{
	int i_counter = 0;
	while(*str != '\0')
	{
		i_counter ++;
		str ++;
	}
	i_counter --;
	str --;
	while(i_counter > 0)
	{
		if(*str == ch)
			return (char*)str;
		
		str --;
	}
	return NULL;
}

char* mystrstr(const char* str1,const char* str2)
{
	char* temp1 = NULL;
	char* temp2 = NULL;
	while(*str1 != '\0')
	{
		temp2 = (char*)str1;
		temp1 = (char*)str2;
		while(*temp1 != '\0')
		{
			if(*str1 != *temp1)
				break;
			temp1 ++;
			str1 ++;
		}
		if(*temp1 == '\0')
		{
			return (char*)temp2;
		}
		else
		{
			str1 ++;
		}
	}
	return NULL;
}

#include <stddef.h> 
/*
	size_t: This is an unsigned integer type used to represent the size of objects in bytes.
	It's often used for memory allocation and manipulation functions.
	size_t buffer_size = 1024;

*/

void *mymemset(void *ptr, int value, size_t num) {
    unsigned char *byte_ptr = ptr;
    unsigned char byte_value = (unsigned char)value;

    for (size_t i = 0; i < num; i++) {
        byte_ptr[i] = byte_value;
    }

    return ptr;
}
